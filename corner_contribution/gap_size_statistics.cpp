#include <iostream>
#include <random>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <omp.h>
#include <stdint.h>
#include <cstdint>
#include <string>
using namespace std;

void get_cluster_gap_sizes(vector<int>& gap_size_statistics, vector<int> cluster, const int L) {

    // Get the starting line and the starting x-posn on the line
    int line = cluster[0] / L;
    int posn = cluster[0] % L;

    bool newline = true;
    // Iterate through every site in the cluster
    for (int i = 1; i < cluster.size(); i++) {

        // Update the position of the newest site
        int gap = cluster[i];
        posn += gap;

        // If that position is on a new line, update line
        if (posn >= L) {
            line += posn/L;
            posn %= L;
            newline = true;
        }
        // Otherwise, add the gap to the statistics
        else {
            if (newline && gap >= L/2) {gap = L - gap;}
            gap_size_statistics[gap-1] += 1;
            newline = false;
        }
    }
}

vector<int> splitString(const string& line) {
    vector<string> result;
    stringstream ss(line);
    string segment;

    while (getline(ss, segment, ' ')) {
        result.push_back(segment);
    }

    vector<int> result_int;
    transform(result.begin(), result.end(), back_inserter(result_int),
                   [](const string& s) { return stoi(s); });
    return result_int;
}

vector<int> get_sample_gap_sizes(const string& filename, const int L) {
    vector<int> gap_size_statistics(L/2, 0);

    ifstream sample_file(filename);
    if (sample_file.is_open()) {
        // File opened successfully, proceed with reading
    } else {
        cerr << "Error: Could not open file." << std::endl;
    }
    string sample_text;

    // Use a while loop together with the getline() function to read the file line by line
    while (getline (sample_file, sample_text)) {
        vector<int> cluster = splitString(sample_text);
        get_cluster_gap_sizes(gap_size_statistics, cluster, L);
    }
    return gap_size_statistics;
}

void run_single_file(const string& input_filename, const string& output_filename, const int L) {
    vector<int> gap_size_statistics = get_sample_gap_sizes(input_filename, L);
    string lines;
    for (const int n: gap_size_statistics) {
        lines += to_string(n) + "\n";
    }

    ofstream file;
    file.open(output_filename);
    file << lines << endl;
    file.close();
}
void run_statistics(const string& input_root, const string& output_root) {
    for (int l: {8, 16, 32, 64, 128}) {
        #pragma omp parallel for num_threads(4)
        for (int run = 0; run < 100; run++) {
            const string input_filename = "./" + input_root + "/" + to_string(l) + "/" + to_string(run) + ".txt";
            const string output_filename = "./" + output_root + "/" + to_string(l) + "/" + to_string(run);
            run_single_file(input_filename, output_filename, l);
        }
    }
}
int main(int argc, const char * argv[]) {
    if (argc != 3) {
        cout << argc << endl;
        return -1;
    }
    const string input = argv[1];
    const string output = argv[2];
    run_statistics(input, output);
    return 0;
}
