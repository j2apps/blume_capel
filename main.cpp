#include <iostream>
#include <array>
#include <math.h>
#include <random>
#include <stack>
#include <vector>
#include <fstream>
#include <chrono>
#include <ranges>
#include <algorithm>
#include <omp.h>
#include <stdlib.h>
#include <sstream>

#ifndef L_MACRO
#define L_MACRO 8
#endif

#ifndef NUM_THREADS
#define NUM_THREADS 1
#endif

using namespace std;


// Ising critical

const double B = 1 / 2.2691853;
const double D = -1000;
const double J = 1;


// Tricritical
/*
const double B = 1 / 0.608;
const double D = 1.966;
const double J = 1;
*/
/*
const double B = 1 / 0.574;
const double D = 1;
const double J = 1;
*/


constexpr int L = static_cast<int>(L_MACRO);
constexpr int N = L*L;
constexpr int dx[4] = {1, L-1, 0, 0};
constexpr int dy[4] = {0, 0, 1, L-1};

random_device rd{};
mt19937 engine{rd()};
array<mt19937, NUM_THREADS> engines;

static uniform_real_distribution<double> p_rand{0.0, 1.0};
static uniform_int_distribution<int> posn_rand{0, L-1};
static uniform_int_distribution<int> fill_rand{-1, 1};
static uniform_int_distribution<int> flip_rand{0, 1};

constexpr int RNG_BATCH_SIZE = 2*L*L*L + 3*L;
constexpr int NUM_METRO_STEPS = 3*N;
array<double, RNG_BATCH_SIZE> rng_buffer;
array<int, NUM_METRO_STEPS> flip_buffer;
int rng_index = RNG_BATCH_SIZE;
int flip_index = NUM_METRO_STEPS;

// Lookup Tables
int modL[2*L];
array<pair<int, int>, L*L> id_to_xy;

struct spin {
    unsigned int value : 2; // 'value' will occupy 2 bits
};

void refill_random() {
    //OPTION 1
    //#pragma omp parallel num_threads(NUM_THREADS)
    {
        // Create thread local distributions
        thread_local uniform_real_distribution<double> lp_rand{0.0, 1.0};
        thread_local uniform_int_distribution<int> lflip_rand{0, 1};

        // Split threads across for loop
        int tid = omp_get_thread_num();
        #pragma omp for schedule(static)
        for (int i = 0; i < rng_index; ++i) {
            rng_buffer[i] = lp_rand(engines[tid]);
        }
        #pragma omp barrier
        #pragma omp for schedule(static)
        for (int i = 0; i < flip_index; ++i) {
            flip_buffer[i] = lflip_rand(engines[tid]);
        }
    }
    rng_index = 0;
    flip_index = 0;
}

int get_posn_id(array<int,2> posn) {
    // Converts posn coord, (x, y), to posn id
    return posn[0] * L + posn[1];
};

array<int, 2> get_posn_from_id(int id) {
    return {id / L, id % L};
}


array<array<int, 2>, 4> nearest_neighbors(array<int,2> posn) {
    // Finds the indices of the 4 nearest neighbors
    const array<array<int, 2>, 4> neighbors = {{
        {(posn[0] + 1) % L, posn[1]},
        {(posn[0] - 1 + L) % L, posn[1]},
        {posn[0], (posn[1] + 1) % L},
        {posn[0], (posn[1] - 1 + L) % L}}
};
    return neighbors;
}

array<int, 4> nearest_neighbor_vals(array<int,2> posn, int (& lattice)[L][L]) {
    //Gets the values of the 4 nearest neighbors
    array<array<int, 2>, 4> neighbors = nearest_neighbors(posn);
    array<int, 4> vals;
    for (int i = 0; i < 4; i++) {
        vals[i] = lattice[neighbors[i][0]][neighbors[i][1]];
    }
    return vals;
}


void metropolis(int (& lattice)[N], array<int,2> posn) {
    // Note the current value, and create a random proposal
    // Proposal have p=0.5 of taking on either of the other values
    const int current = lattice[posn[0]*L + posn[1]];
    const int proposal = (current + 2 + flip_buffer[flip_index++]) % 3 - 1;

    // Calculate the change in energy (from the blume-capel hamiltonian)
    int couple = 0;
    for (int d = 0; d < 4; d++) {
        const int n_id = modL[(posn[0] + dx[d])]*L + modL[(posn[1] + dy[d])];
        couple += lattice[n_id];
    }
    const int delta_e = - J * couple * (proposal - current) + D * (proposal*proposal - current*current);

    // Accept/reject proposal based on the change in energy
    if (rng_buffer[rng_index++] < exp (-B * delta_e)) {
        lattice[posn[0]*L + posn[1]] = proposal;
    }
}
void wolff(int (&lattice)[N]) {
    // Bond formation probability
    static const double p = 1 - exp(-2 * B * J);

    array<int, N> vst;
    array<int, N> hst;
    int vst_index = 0, vst_head = 0;
    int hst_index = 0, hst_head = 0;

    // Pick a non-zero random site
    int value = 0, count = 0;
    int seed_x = 0, seed_y = 0;

    // Generate a random site to seed the cluster
    while (value == 0 && count < N) {
        seed_x = posn_rand(engine);
        seed_y = posn_rand(engine);
        value = lattice[seed_x + seed_y * L];
        ++count;
    }
    if (value == 0) return;

    // Seed and flip the starting site
    const int flipped = -value;
    lattice[seed_x + seed_y * L] = flipped;

    // Horizontal stack marks sites that still must search horizontally
    hst[0] = seed_x + seed_y * L;
    // Vertical stack marks sites that have already been searched horizontally
    vst[0] = seed_x + seed_y * L;

    // Algorithm searches horz first, then vert
    // This limits re-checks horizontally and optimizes cache locality
    while (vst_index <= vst_head) {
        while (hst_index <= hst_head) {
            const int anchor = hst[hst_index++];
            const int x = anchor % L;
            const int y = anchor / L;
            // Attempt to bond to your right-neighbor until failure
            for (int i = 1; i < L; i++) {
                const int n_id = modL[x + i] + (y * L);
                // Probablistically form bond with neighbor if they share the same spin
                if (lattice[n_id] == value && rng_buffer[rng_index++] < p) {
                    lattice[n_id] = flipped;
                    vst[++vst_head] = n_id;
                }
                else {
                    break;
                }
            }
            // Attempt to bond to your left-neighbor until failure
            for (int i = 1; i < L; i++) {
                const int n_id = modL[x - i] + (y * L);
                // Probablistically form bond with neighbor if they share the same spin
                if (lattice[n_id] == value && rng_buffer[rng_index++] < p) {
                    lattice[n_id] = flipped;
                    vst[++vst_head] = n_id;
                }
                else {
                    break;
                }
            }
        }
        // After the horizontal stack is exhausted, move to vertical stack
        const int id = vst[vst_index++];
        const int x = id % L;
        const int y = id / L;

        // Check upstairs neighbor
        int n_id = x + modL[(y + 1)] * L;
        int n_val = lattice[n_id];
        if (n_val == value && rng_buffer[rng_index++] < p) {
            lattice[n_id] = flipped;
            hst[++hst_head] = n_id;
        }
        // Check downstairs neighbor
        n_id = x + modL[(y + L - 1)] * L;
        n_val = lattice[n_id];
        if (n_val == value && rng_buffer[rng_index++] < p) {
            lattice[n_id] = flipped;
            hst[++hst_head] = n_id;
        }
    }
}

void step(int (&lattice)[N]) {
    // Perform 3N single-site Metropolis steps and L single-cluster Wolff steps
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < L; j++) {
            for (int k = 0; k < L; k++) {
                metropolis(lattice, {j, k});
            }
        }
    }
    // Perform L Wolff steps
    for (int i = 0; i < L; i++) {
        wolff(lattice);
    }
}

void generate_lattice(int (& lattice)[N]) {
    // Fills the lattice randomly
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            lattice[i*L + j] = fill_rand(engine);
        }
    }
}

void generate_ising_lattice(int (& lattice)[N]) {
    static uniform_int_distribution<int> ising_fill_rand{-1, 0};
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            int fill = ising_fill_rand(engine);
            if (fill == 0) {
                fill = 1;
            }
            lattice[i*L + j] = fill;
        }
    }
}

class Cluster {
public:
    bool sign;
    vector<int> sites;
};

vector<Cluster> form_clusters(int (&lattice)[L][L], double bond_prob) {
    // Copy lattice
    int lattice2[L][L];
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            lattice2[i][j] = lattice[i][j];
        }
    }

    // List of clusters
    vector<Cluster> clusters;

    // Loop through every site in the lattice
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {

            // Ignore sites if they are empty or are in a cluster already
            const int value = lattice2[i][j];
            if (value == 0) {continue;}

            // Stack of sites and cluster
            stack<array<int, 2>> st;
            st.push({i, j});

            Cluster cluster;
            cluster.sign = value == 1;
            cluster.sites.push_back(get_posn_id({i, j}));

            // Set sites that are added to a cluster to 0 so they are not added again
            lattice2[i][j] = 0;
            array<int, 2> site;

            // Go through stack and look at all nearest neighbors
            while (!st.empty()) {
                site = st.top();
                st.pop();

                // Find nearest neighbors
                array<array<int,2>,4> neighbors = nearest_neighbors(site);
                for (array neighbor : neighbors) {
                    // Check if each neighbor has the value of the clusters
                    // Do a probability check
                    if (lattice2[neighbor[0]][neighbor[1]] == value && p_rand(engine) < bond_prob) {
                        // If same value and probability check, add to cluster and stack
                        st.push(neighbor);
                        cluster.sites.push_back(get_posn_id(neighbor));

                        // Set sites that are added to a cluster to 0 so they are not added again
                        lattice2[neighbor[0]][neighbor[1]] = 0;
                    }
                }
            }
            if (cluster.sites.size() > 0) {
                clusters.push_back(cluster);
            }
        }
    }
    return clusters;
}

bool sort_key(int a, int b) {
    // Sorts coordinates by their ids
    return a < b;
}

void export_clusters(int (&lattice)[N], double p,  bool sign, string filename) {
    // Exports clusters to a file
    //unflatten
    int lattice_2d[L][L];
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            lattice_2d[i][j] = lattice[i*L + j];
        }
    }
    // Get the clusters using form_clusters
    vector<Cluster> clusters = form_clusters(lattice_2d, p);

    // Initialize output
    string lines;

    // Iterate through every cluster
    for (Cluster cluster : clusters) {
        // Sort clusters
        sort(cluster.sites.begin(), cluster.sites.end(), sort_key);
        string line;
        // Include sign if specified
        if (sign) {
            line = cluster.sign ? "+ " : "- ";
        }
        // Write each cluster in gap format as a single line deliminated by spaces
        int previous = 0;
        for (int posn : cluster.sites) {
            line += to_string(posn - previous) + " ";
            previous = posn;
        }
        lines += line + "\n";
    }
    // Output to file
    ofstream file;
    file.open(filename);
    file << lines << endl;
    file.close();
}
Cluster generate_cluster_from_string(const string& line) {
    vector<string> result;
    stringstream ss(line);
    string segment;
    bool sign = false;
    while (getline(ss, segment, ' ')) {
        result.push_back(segment);
    }
    if (!result.empty()) {
        sign = result[0] == "+";
        result.erase(result.begin());
    }
    vector<int> result_int;
    transform(result.begin(), result.end(), back_inserter(result_int),
                   [](const string& s) { return stoi(s); });

    Cluster c;
    c.sign = sign;
    c.sites = result_int;
    return c;
}

void get_lattice_from_burn(int (&lattice)[N], string burn) {
    string sample_text;
    ifstream sample_file(burn);
    // Use a while loop together with the getline() function to read the file line by line
    while (getline(sample_file, sample_text)) {
        Cluster c = generate_cluster_from_string(sample_text);
        if (c.sites.size() == 0) {
            continue;
        }
        int prev = c.sites[0];
        lattice[prev] = c.sign ? 1 : -1;
        for (int i = 1; i < c.sites.size(); i++) {
            int current = prev + c.sites[i];
            prev = current;
            lattice[current] = c.sign ? 1 : -1;
        }
    }
}

int main(int argc, const char * argv[]) {

    for (int i = 0; i < 2*L; i++) {
        modL[i] = i % L;
    }

    for (int x = 0; x < L; ++x) {
        for (int y = 0; y < L; ++y) {
            id_to_xy[x * L + y] = {x, y};
        }
    }

    // test_suite();
    // return 0;
    // Seed the thread-specific rngs
	for (int i = 0; i < NUM_THREADS; i++) {
		engines[i].seed(random_device{}());
	}

    // Get the run and directory of clusters from command-line arguments
    int run;
    string root;
    string burn;
    if (argc > 1) {
        run = atoi(argv[1]);
        root = argv[2];
        burn = atoi(argv[3]);
    }
    else {
        run = 0;
        root = "NONE";
        burn = "NONE";
    }

    // Initialize and populate the lattice
    int lattice[N];
    //generate_lattice(lattice);

    get_lattice_from_burn(lattice, burn);

/*
    for (int i = 0; i < 1500*N; i++) {
        #pragma omp parallel num_threads(NUM_THREADS)
        {
            refill_random();
        }
        step(lattice);
    }
    export_clusters(lattice, 1, true,
            "./" + root + "/" + to_string(L) + "_burn.txt");
    return 0;
*/
    // Data collection of 9*1500N steps
    for (int i = 0; i < 1500; i++) {
        for (int j = 0; j < 9 * L*L; j++) {
            #pragma omp parallel num_threads(NUM_THREADS)
            {
                refill_random();
            }
            step(lattice);
        }
        // Export the data to text files
        export_clusters(lattice, 1, true,
            "./" + root + "/spin/" + to_string(L) + "/" + to_string(run) + "/" + to_string(i) + ".txt");
        export_clusters(lattice, 1 - exp (-2 * B * J), true,
            "./" + root + "/fk/" + to_string(L) + "/" + to_string(run) + "/" + to_string(i) + ".txt");
    }
    return 0;
}
