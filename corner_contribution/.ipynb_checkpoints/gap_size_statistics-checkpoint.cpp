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
#include <filesystem>
#include <string>
#include <array>

using namespace std;
namespace fs = std::filesystem;

#ifndef NUM_THREADS
#define NUM_THREADS 1
#endif

void get_cluster_gap_sizes_old(vector<int>& gap_size_statistics, vector<vector<int>> cluster, const int L) {
    for (const auto& line : cluster) {
        int n = line.size();
        if (n < 2) continue;

        // Count all gaps
        for (int k = 0; k < n; ++k) {
            int a = line[k];
            int b = line[(k + 1) % n];
            int gap = (b - a + L) % L;

            if (gap > L/2) gap = L - gap;
            gap_size_statistics[gap - 1]++;
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
    if (!result.empty()) {
        result.erase(result.begin());
    }
    vector<int> result_int;
    transform(result.begin(), result.end(), back_inserter(result_int),
                   [](const string& s) { return stoi(s); });
    return result_int;
}

// TODO: FIX THIS FUNCTION
void get_cluster_gss(vector<int> &cluster, vector<int> &gap_size_statistics, const int L) {
    // Get the starting position from the cluster
    int first_posn = cluster[0] % L;
    int last_posn = first_posn;

    for (int i = 1; i < cluster.size(); ++i) {
        // Wrap-around case
        if (last_posn + cluster[i] >= L) {
            int gap = last_posn - first_posn;
            if (gap > 0) {
                if (gap > L/2) gap = L - gap;
                gap_size_statistics[gap - 1]++;
            }
            last_posn += cluster[i];
            last_posn %= L;
            first_posn = last_posn;
        }
        // Normal case
        else {
            int gap = cluster[i];
            if (gap > L/2) gap = L - gap;
            gap_size_statistics[gap - 1]++;
            last_posn += cluster[i];
        }
    }
    // Last line of file
    int gap = last_posn - first_posn;
    if (gap > 0) {
        if (gap > L/2) gap = L - gap;
        gap_size_statistics[gap - 1]++;
    }
}

void get_sample_gap_sizes(vector<int>& gap_size_statistics, const string& filename, const int L) {
    ifstream sample_file(filename);
    if (sample_file.is_open()) {
        // File opened successfully, proceed with reading
    } else {
        cerr << "Error: Could not open file." << std::endl;
    }
    string sample_text;

    // Use a while loop together with the getline() function to read the file line by line
    while (getline(sample_file, sample_text)) {
        vector<int> cluster = splitString(sample_text);
        if (cluster.empty()) {continue;}
        // vector<vector<int>> lines = convert_cluster_to_lines(cluster, L);
        // get_cluster_gap_sizes(gap_size_statistics, lines, L);
        get_cluster_gss(cluster, gap_size_statistics, L);
    }
}
int countSubdirectories(const fs::path& directoryPath) {
    int count = 0;
    try {
        if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
            cerr << "Error: Path does not exist or is not a directory." << endl;
            return -1; // Indicate an error
        }

        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            if (fs::is_directory(entry.status())) {
                count++;
            }
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
        return -1; // Indicate an error
    }
    return count;
}

void run_single_run(const string& input_dirname, const string& output_filename, const int L) {
    // Initialize gap_size_statistics with size L/2, since there are L/2 possible gap sizes
    vector<int> gap_size_statistics(L/2, 0);
    int num_samples = 0;
    // Find all files in the directory, get the gap sizes from each, and update num_samples
    for (const auto & entry : fs::directory_iterator(input_dirname)) {
	    string filepath = entry.path().string();
        get_sample_gap_sizes(gap_size_statistics, filepath, L);
        num_samples ++;
    }
    // Output data in the format:
    // #samples
    // #1gap #2gap #3gap...
    string output = to_string(num_samples) + "\n";
    for (const int n: gap_size_statistics) {
        output += to_string(n) + " ";
    }

    // Output to file
    ofstream file;
    file.open(output_filename);
    file << output << endl;
    file.close();
}

void run_statistics(const string& input_root, const string& output_root, const int nruns) {
    for (int l: {16, 24, 32, 48, 64, 96}) {
        // Write string ahead of time to avoid race conditions
	    vector<string> input_dirnames(nruns);
	    vector<string> output_filenames(nruns);
	    for (int run=0; run<nruns; run++) {
		    input_dirnames[run] = input_root + "/" + to_string(l) + "/" + to_string(run);
		    output_filenames[run] = output_root + "/" + to_string(l) + "/" + to_string(run) + ".txt";
	    }
        // Split runs up between threads
        #pragma omp parallel for num_threads(NUM_THREADS)
        for (int run = 0; run < nruns; run++) {
            run_single_run(input_dirnames[run], output_filenames[run], l);
        }
    }
}
int main(int argc, const char * argv[]) {
    // Ensure the correct arguments are in place
    
    if (argc != 4) {
        cout << "Wrong number of args:" << endl;
        cout << argc << endl;
        return -1;
    }

    const string input = argv[1];
    const string output = argv[2];
    const int nruns = atoi(argv[3]);
    run_statistics(input, output, nruns);
    return 0;
    
    //run_single_run("./sample_data/L64_Ising", "./sample_data/gap64.txt", 64);
}
