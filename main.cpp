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
const int L = static_cast<int>(L_MACRO);

random_device rd{};
mt19937 engine{rd()};
array<mt19937, NUM_THREADS> engines;

static uniform_real_distribution<double> p_rand{0.0, 1.0};
static uniform_int_distribution<int> posn_rand{0, L-1};
static uniform_int_distribution<int> fill_rand{-1, 1};

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
    int current = lattice[posn[0]][posn[1]];
    int proposal = (current + 2 + flip) % 3 - 1;

    // Get the values of the 4 nearest neighbors
    array<int, 4> vals = nearest_neighbor_vals(posn, lattice);

    // Calculate the change in energy (from the blume-capel hamiltonian)
    int couple = accumulate(vals.begin(), vals.end(), 0);
    int delta_e = J * couple * (proposal - current) + D * (proposal*proposal - current*current);

    // Accept/reject proposal based on the change in energy
    if (exp (-B * delta_e) > p) {
        lattice[posn[0]][posn[1]] = proposal;
    }
}

void wolff(int (&lattice)[L][L]) {
    // Probability of new bonds in a cluster being formed
    double p = 1 - exp (-2 * B * J);

    // Declare stack and cluster
    stack<array<int,2>> st;

    // Start a count, so that empty lattices do not get stuck
    int count = 0;

    // Note the value of the site
    int value = 0;
    array<int, 2> site;

    // Find a site that is non-empty
    while (value == 0) {
        site = {posn_rand(engine), posn_rand(engine)};
        value = lattice[site[0]][site[1]];
        count +=1;
        if (count >= L*L) {return;}
    }

    // Flip the value at that site
    // Add the site to the stack and cluster
    lattice[site[0]][site[1]] = -value;
    st.push(site);

    // Loop through the stack
    while (!st.empty()) {
        site = st.top();
        st.pop();

        // Add nearest neighbors with same value with probability p
        array<array<int,2>,4> neighbors = nearest_neighbors(site);
        for (int i = 0; i < 4; i++) {
            if (lattice[neighbors[i][0]][neighbors[i][1]] == value && p_rand(engine) < p) {
                st.push(neighbors[i]);
                lattice[neighbors[i][0]][neighbors[i][1]] = -value;
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
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        // Create thread local distributions
        thread_local uniform_real_distribution<double> lp_rand{0.0, 1.0};
        thread_local uniform_int_distribution<int> lposn_rand{0, L-1};
        thread_local uniform_int_distribution<int> lflip_rand{0, 1};

        // Get the thread number
        int tid = omp_get_thread_num();

        // Split threads across for loop
        #pragma omp for schedule(static)
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
                metropolis(lattice, {j, k}, flip_rands[i], p_rands[i]);
            }
        }
    }
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


int get_posn_id(array<int,2> posn) {
    // Converts posn coord, (x, y), to posn id
    return posn[0] * L + posn[1];
};

vector<vector<array<int, 2>>> form_clusters(int (&lattice)[L][L], double p) {
    // Copy lattice
    int lattice2[L][L];
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            lattice2[i][j] = lattice[i][j];
        }
    }

    // List of clusters
    vector<vector<array<int, 2>>> clusters;

    // Loop through every site in the lattice
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {

            // Ignore sites if they are empty or are in a cluster already
            const int value = lattice2[i][j];
            if (value == 0) {continue;}

            // Stack of sites and cluster
            stack<array<int, 2>> st;
            vector<array<int, 2>> cluster;
            st.push({i, j});
            cluster.push_back({i, j});

            // Set sites that are added to a cluster to 0 so they are not added again
            lattice2[i][j] = 0;

            array<int, 2> site{};

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
                        cluster.push_back(neighbor);

                        // Set sites that are added to a cluster to 0 so they are not added again
                        lattice2[neighbor[0]][neighbor[1]] = 0;
                    }
                }
            }
            if (cluster.size() > 1) {
                clusters.push_back(cluster);
            }
        }
    }
    return clusters;
}

bool sort_key(array<int, 2> a, array<int,2> b) {
    // Sorts coordinates by their ids
    return get_posn_id(a) < get_posn_id(b);
}

void export_clusters(int (&lattice)[L][L], double p, string filename) {
    // Exports clusters to a file

    // Get the clusters using form_clusters
    vector<vector<array<int, 2>>> clusters = form_clusters(lattice, p);

    // Initialize output
    string lines;

    // Iterate through every cluster
    for (vector cluster : clusters) {
        // Sort clusters
        sort(cluster.begin(), cluster.end(), sort_key);
        string line;

        // Write each cluster in gap format as a single line deliminated by spaces
        int previous = 0;
        for (array posn : cluster) {
            int posn_id = get_posn_id(posn);
            line += to_string(posn_id - previous) + " ";
            previous = posn_id;
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
    for (int i = 0; i < 1500 * L*L; i++) {
        step(lattice);
    }

    auto end = chrono::high_resolution_clock::now();
    double duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    cout << NUM_THREADS << " " << duration / 1000.0 << endl;

    // Data collection of 9*1500N steps
    for (int i = 0; i < 1500; i++) {
        for (int j = 0; j < 9 * L*L; j++) {
            step(lattice);
        }
        // Export the data to text files
        export_clusters(lattice, 1,
            "./" + root + "/spin/" + to_string(L) + "/" + to_string(run) + "/" + to_string(i) + ".txt");
        export_clusters(lattice, 1 - exp (-2 * B * J),
            "./" + root + "/fk/" + to_string(L) + "/" + to_string(run) + "/" + to_string(i) + ".txt");
    }
    return 0;
}
