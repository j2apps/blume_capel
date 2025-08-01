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
			if (gap >= L/2) {
			    // If big gap and new line, then it is a wrap around, and
				if (newline) {
					gap = L - gap;
				}
				else {
					continue;
				}
			}
            gap_size_statistics[gap-1] += 1;
            newline = false;
        }
    }
}
void get_cluster_gap_sizes(vector<int>& gap_size_statistics, vector<vector<int>> cluster, const int L) {
    for (vector<int> line: cluster) {
        bool largest = false;

        // Special case for only 2 sites
        if (line.size() == 2) {
            int gap = line[1] - line[0];
            gap = min(gap, L - gap);
            gap_size_statistics[gap-1] ++;
            continue;
        }
        // If more than 2 sites, procede
        // Compute the gaps, include the one between the first and last element
        for (int i = 0; i < line.size() - 1; i++) {
            int gap = line[i+1] - line[i];
            gap = min(gap, L - gap);
            gap_size_statistics[gap-1] ++;
        }
        // Get the last and first as well
        int gap = line[line.size() - 1] - line[0];
        gap = min(gap, L - gap);
        gap_size_statistics[gap-1] ++;
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

vector<vector<int>> convert_cluster_to_lines(vector<int> cluster, const int L) {
    // Allocate result vector
    vector<vector<int>> result;
    // Get the starting position from the cluster
    int posn = cluster[0] / L;
    vector<int> line;
    for (int i = 1; i < cluster.size(); i++) {
        // Find the updated posn based on the gap
        posn += cluster[i];
        // If the posn is on the next line, update line and posn
        if (posn >= L) {
            // If the line has more than one site, append it to result
            if (line.size() > 1) {
                result.push_back(line);
            }
            line.clear();
            posn %= L;
        }
        // Append posn to the appropriate line
        line.push_back(posn);
    }
    // Append remaining line
    if (line.size() > 1) {
        result.push_back(line);
    }
    return result;
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
        vector<vector<int>> lines = convert_cluster_to_lines(cluster, L);
        get_cluster_gap_sizes(gap_size_statistics, lines, L);
    }
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

void run_statistics(const string& input_root, const string& output_root) {
    constexpr int nruns = 25;
    for (int l: {16}) {
        // Write string ahead of time to avoid race conditions
	    array<string, nruns> input_dirnames;
	    array<string, nruns> output_filenames;
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
    if (argc != 3) {
        cout << argc << endl;
        return -1;
    }

    const string input = argv[1];
    const string output = argv[2];
    run_statistics(input, output);
    return 0;
}
