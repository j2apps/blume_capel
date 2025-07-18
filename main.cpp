#include <iostream>
#include <array>
#include <numeric>
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
#include <queue>

#ifndef L_MACRO
#define L_MACRO 8
#endif

#ifndef NUM_THREADS
#define NUM_THREADS 1
#endif

#define NUM_METRO_STEPS  (3 * L_MACRO * L_MACRO)

using namespace std;


// Ising critical
const double B = 1 / 2.2691853;
const double D = -1000;
const double J = 1;

/*
// Tricritical
const double B = 1 / 0.608;
const double D = 1.966;
const double J = 1;
*/

constexpr int L = static_cast<int>(L_MACRO);

random_device rd{};
mt19937 engine{rd()};
array<mt19937, NUM_THREADS> engines;

static uniform_real_distribution<double> p_rand{0.0, 1.0};
static uniform_int_distribution<int> posn_rand{0, L-1};
static uniform_int_distribution<int> fill_rand{-1, 1};

constexpr int RNG_BATCH_SIZE = 2*L*L*L;
array<double, RNG_BATCH_SIZE> rng_buffer;
int rng_index = RNG_BATCH_SIZE;

// Lookup Tables
int modL[2*L];
array<pair<int, int>, L*L> id_to_xy;

double get_next_random() {
    if (rng_index >= RNG_BATCH_SIZE) {
        #pragma omp parallel num_threads(NUM_THREADS)
        {
            // Create thread local distributions
            thread_local uniform_real_distribution<double> lp_rand{0.0, 1.0};

            // Split threads across for loop
            int tid = omp_get_thread_num();
            #pragma omp for schedule(static)
            for (int i = 0; i < RNG_BATCH_SIZE; ++i) {
                rng_buffer[i] = lp_rand(engines[tid]);
            }
        }
        rng_index = 0;
    }
    return rng_buffer[rng_index++];
}
void refill_random() {
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        // Create thread local distributions
        thread_local uniform_real_distribution<double> lp_rand{0.0, 1.0};

        // Split threads across for loop
        int tid = omp_get_thread_num();
        #pragma omp for schedule(static)
        for (int i = 0; i < rng_index; ++i) {
            rng_buffer[i] = lp_rand(engines[tid]);
        }
    }
    rng_index = 0;
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


void metropolis(int (& lattice)[L][L], array<int,2> posn, int flip, double p) {
    // Note the current value, and create a random proposa
    // Proposal have p=0.5 of taking on either of the other values
    const int current = lattice[posn[0]][posn[1]];
    const int proposal = (current + 2 + flip) % 3 - 1;

    // Get the values of the 4 nearest neighbors
    array<int, 4> vals = nearest_neighbor_vals(posn, lattice);

    // Calculate the change in energy (from the blume-capel hamiltonian)
    const int couple = accumulate(vals.begin(), vals.end(), 0);
    const int delta_e = - J * couple * (proposal - current) + D * (proposal*proposal - current*current);

    // Accept/reject proposal based on the change in energy
    if (exp (-B * delta_e) > p) {
        lattice[posn[0]][posn[1]] = proposal;
    }
}
void wolff(int (&lattice)[L][L]) {
    // Bond formation probability
    static const double p = 1 - exp(-2 * B * J);

    // Directions for nearest neighbor
    constexpr int dx[4] = {1, L-1, 0, 0};
    constexpr int dy[4] = {0, 0, 1, L-1};

    array<int, L_MACRO * L_MACRO> st;
    int st_index = 0, st_head = 0;

    // Pick a non-zero random site
    int value = 0, count = 0;
    int seed_x = 0, seed_y = 0;

    while (value == 0 && count < L * L) {
        seed_x = posn_rand(engine);
        seed_y = posn_rand(engine);
        value = lattice[seed_x][seed_y];
        ++count;
    }
    if (value == 0) return;

    // Flip and seed
    lattice[seed_x][seed_y] = -value;
    st[st_head++] = (seed_x * L + seed_y);

    size_t index = 0;

    // Process cluster
    while (st_index < st_head) {
        int id = st[st_index++];
        //const auto [x, y] = id_to_xy[id];
        const int x = id / L;
        const int y = id % L;

        for (int d = 0; d < 4; ++d) {
            const int nx = modL[(x + dx[d])];
            const int ny = modL[(y + dy[d])];

            if (lattice[nx][ny] == value && p > rng_buffer[rng_index++]) {
                lattice[nx][ny] = -value;
                st[st_head++] = (nx * L + ny);
            }
        }
    }
}

void step(int (&lattice)[L][L]) {
    // Initialize arrays of random numbers to be fed to Metropolis
    array<array<int, 2>, NUM_METRO_STEPS> posn_rands;
    array<int, NUM_METRO_STEPS> flip_rands;
    array<double, NUM_METRO_STEPS> p_rands;

    // Generate random numbers in parallel
    //#pragma omp parallel num_threads(NUM_THREADS)
    {
        // Create thread local distributions
        thread_local uniform_real_distribution<double> lp_rand{0.0, 1.0};
        thread_local uniform_int_distribution<int> lposn_rand{0, L-1};
        thread_local uniform_int_distribution<int> lflip_rand{0, 1};

        // Get the thread number
        int tid = omp_get_thread_num();

        // Split threads across for loop
        //#pragma omp for schedule(static)
        for (int i=0; i < NUM_METRO_STEPS; i++) {
            // Use local distributions and thread-specific engines to generate random numbers
            flip_rands[i] = lflip_rand(engines[tid]);
            p_rands[i] = lp_rand(engines[tid]);
        }
    }
    // Perform 3N single-site Metropolis steps and L single-cluster Wolff steps
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < L; j++) {
            for (int k = 0; k < L; k++) {
                const int id = i*L*L + j*L + k;
                metropolis(lattice, {j, k}, flip_rands[id], p_rands[id]);
            }
        }
    }
    refill_random();
    for (int i = 0; i < L; i++) {
        wolff(lattice);
    }
}

void generate_lattice(int (& lattice)[L][L]) {
    // Fills the lattice randomly
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            lattice[i][j] = fill_rand(engine);
        }
    }
}

void generate_ising_lattice(int (& lattice)[L][L]) {
    static uniform_int_distribution<int> ising_fill_rand{-1, 0};
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            int fill = ising_fill_rand(engine);
            if (fill == 0) {
                fill = 1;
            }
            lattice[i][j] = fill;
        }
    }
}

class Cluster {
public:
    bool sign;
    vector<int> sites;
};

vector<Cluster> form_clusters(int (&lattice)[L][L], double p) {
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
                    if (lattice2[neighbor[0]][neighbor[1]] == value && p_rand(engine) < p) {
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

void export_clusters(int (&lattice)[L][L], double p,  bool sign, string filename) {
    // Exports clusters to a file

    // Get the clusters using form_clusters
    vector<Cluster> clusters = form_clusters(lattice, p);

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

int test_suite() {
    int lattice[L][L];
    generate_lattice(lattice);
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            cout << lattice[i][j] << " ";
        }
        cout<< endl;
    }
    cout << endl;
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            array<array<int,2>, 4> neighbors = nearest_neighbors({i, j});
            array<int, 4> vals = nearest_neighbor_vals({i, j}, lattice);
            cout << to_string(i) + ", " + to_string(j) + ":\n";
            for (int k=0; k<4; k++) {
                cout << vals[k] << " | " << neighbors[k][0] << ", " << neighbors[k][1] << endl;
            }
            cout << endl;

        }
    }
    return 0;
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
    if (argc > 1) {
        run = atoi(argv[1]);
        root = argv[2];
    }
    else {
        run = 0;
        root = "NONE";
    }

    // Initialize and populate the lattice
    int lattice[L][L];
    generate_ising_lattice(lattice);

    auto start = chrono::high_resolution_clock::now();

    // Burn in of 1500N steps
    for (int i = 0; i < 1500*L*L; i++) {
        step(lattice);
    }

    auto end = chrono::high_resolution_clock::now();
    double duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    cout << to_string(L) + "/" + to_string(run) << ": " << duration / 1000.0 << endl;

    // Data collection of 9*1500N steps
    for (int i = 0; i < 1500; i++) {
        for (int j = 0; j < 9 * L*L; j++) {
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
