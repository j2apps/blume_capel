#include <iostream>
#include <array>
#include <numeric>
#include <math.h>
#include <random>
#include <stack>
#include <time.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ranges>
using namespace std;


const double B = 1 / 2.2691853;
const double D = -1000;
const double J = 1;
const int L = static_cast<int>(L_MACRO);

random_device rd{};
mt19937 engine{rd()};
uniform_real_distribution<double> p_rand{0.0, 1.0};
uniform_int_distribution<int> posn_rand{0, L-1};
uniform_int_distribution<int> flip_rand{0, 1};
uniform_int_distribution<int> fill_rand{-1, 1};

array<array<int, 2>, 4> nearest_neighbors(array<int,2> posn) {
    array<array<int, 2>, 4> neighbors = {{
        {(posn[0] + 1) % L, posn[1]},
        {(posn[0] - 1 + L) % L, posn[1]},
        {posn[0], (posn[1] + 1) % L},
        {posn[0], (posn[1] - 1 + L) % L}}
};
    return neighbors;
}
array<int, 4> nearest_neighbor_vals(array<int,2> posn, int (& lattice)[L][L]) {
    array<array<int, 2>, 4> neighbors = nearest_neighbors(posn);
    array<int, 4> vals = {0,0,0,0};
    for (int i = 0; i < 4; i++) {
        vals[i] = lattice[neighbors[i][0]][neighbors[i][1]];
    }
    return vals;
}


void metropolis(int (& lattice)[L][L]) {
    array<int, 2> posn = {posn_rand(engine), posn_rand(engine)};
    int current = lattice[posn[0]][posn[1]];
    int proposal = (current + 2 + flip_rand(engine)) % 3 - 1;

    array<int, 4> vals = nearest_neighbor_vals(posn, lattice);

    int couple = accumulate(vals.begin(), vals.end(), 0);
    int delta_e = couple * (proposal - current) + D * (pow(proposal,2) - pow(current,2));

    if (exp (-B * delta_e) > p_rand(engine)) {
        lattice[posn[0]][posn[1]] = proposal;
    }
}

void wolff(int (&lattice)[L][L]) {
    double p = 1 - exp (-2 * B * J);

    // Declare stack and cluster
    stack<array<int,2>> st;
    stack<array<int,2>> cluster;

    int count = 0;
    int value = 0;
    array<int, 2> site;

    while (value == 0) {
        site = {posn_rand(engine), posn_rand(engine)};
        value = lattice[site[0]][site[1]];
        count +=1;
        if (count >= pow(L,2)) {return;}
    }

    lattice[site[0]][site[1]] = -value;

    st.push(site);
    cluster.push(site);

    while (!st.empty()) {
        site = st.top();
        st.pop();

        array<array<int,2>,4> neighbors = nearest_neighbors(site);
        for (int i = 0; i < 4; i++) {
            if (lattice[neighbors[i][0]][neighbors[i][1]] == value && p_rand(engine) < p) {
                st.push(neighbors[i]);
                cluster.push(neighbors[i]);
                lattice[neighbors[i][0]][neighbors[i][1]] = -value;
            }
        }
    }
}

void step(int (&lattice)[L][L]) {
    for (int i = 0; i < 3 * pow(L,2); i++) {
        metropolis(lattice);
    }
    for (int i = 0; i < L; i++) {
        wolff(lattice);
    }
}

void generate_lattice(int (& lattice)[L][L]) {
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            lattice[i][j] = fill_rand(engine);
        }
    }
}


int get_posn_id(array<int,2> posn) {
    return posn[0] * L + posn[1];
};

vector<vector<array<int, 2>>> form_clusters(int (&lattice)[L][L], double p) {
    // copy lattice
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
            clusters.push_back(cluster);
        }
    }
    return clusters;
}

bool sort_key(array<int, 2> a, array<int,2> b) {
    return get_posn_id(a) < get_posn_id(b);
}

void export_clusters(int (&lattice)[L][L], double p, string filename) {
    vector<vector<array<int, 2>>> clusters = form_clusters(lattice, p);
    string lines;

    for (vector cluster : clusters) {
        sort(cluster.begin(), cluster.end(), sort_key);
        string line;
        int previous = 0;
        for (array posn : cluster) {
            int posn_id = get_posn_id(posn);
            line += to_string(posn_id - previous) + " ";
            previous = posn_id;
        }
        lines += line + "\n";
    }
    ofstream file;
    file.open(filename);
    file << lines << endl;
    file.close();
}

int main(int argc, const char * argv[]) {
    const int run = atoi(argv[1]);
    int lattice[L][L];
    generate_lattice(lattice);

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < 1500 * pow(L,2); i++) {
        step(lattice);
    }

    auto end = chrono::high_resolution_clock::now();
    double duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    cout << duration / 1000.0 << endl;

    /*for (int i = 0; i < 1500; i++) {
        for (int j = 0; j < 9 * pow(L,2); j++) {
            step(lattice);
        }
        string base = "clusters";
        export_clusters(lattice, 1,
            "./" + base + "/spin/" + to_string(L) + "/" + to_string(run) + "/" + to_string(i) + ".txt");
        export_clusters(lattice, 1 - exp (-2 * B * J),
            "./" + base + "/fk/" + to_string(L) + "/" + to_string(run) + "/" + to_string(i) + ".txt");
    }*/



    return 0;
}