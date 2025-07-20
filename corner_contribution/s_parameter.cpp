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


int count_size(const string& line) {
    stringstream ss(line);
    string segment;
    int count = 0;
    while (getline(ss, segment, ' ')) {
        count ++;
    }
    if (count > 1) {
        return count - 1;
    }

}

void get_sample_statistics(const string& filename) {
    ifstream sample_file(filename);
    if (sample_file.is_open()) {
        // File opened successfully, proceed with reading
    } else {
        cerr << "Error: Could not open file." << std::endl;
    }
    string sample_text;
    vector<int> sizes;
    // Use a while loop together with the getline() function to read the file line by line
    while (getline(sample_file, sample_text)) {
        size = count_size(sample_text);
        if (size == 0) {
        continue;
        }
        sizes.push_back(size);
    }
    int s = 0;
    for (int size: sizes) {
        s += size*size;
    }
    s -= max(sizes);
    return s;
}

int run_single_run(const string& input_dirname, const string& output_filename, const int L) {
    // Initialize gap_size_statistics with size L/2, since there are L/2 possible gap sizes
    int s_total;
    int num_samples = 0;
    // Find all files in the directory, get the gap sizes from each, and update num_samples
    for (const auto & entry : fs::directory_iterator(input_dirname)) {
	    string filepath = entry.path().string();
        s_total += get_sample_statistics(filepath);
        num_samples ++;
    }
    return s_total/num_samples;
}

double stdev(const std::vector<int>& data) {
    double sum = 0.0;
    double mean, standardDeviation = 0.0;

    for (int value : data) {
        sum += value;
    }
    mean = sum / data.size();

    for (int value : data) {
        standardDeviation += (value - mean) * (value - mean);
    }

    return sqrt(standardDeviation / data.size());
}
double mean(const std::vector<int>& data) {
    double sum = 0.0;
    for (int value : data) {
        sum += value;
    }
    return sum / data.size();
}

void run_statistics(const string& input_root, const string& output_root) {
    string output;
    for (int l: {16, 32, 64}) {
        // Write string ahead of time to avoid race conditions
	    array<string, 100> input_dirnames;
	    for (int run=0; run<100; run++) {
		    input_dirnames[run] = input_root + "/" + to_string(l) + "/" + to_string(run);
	    }

        // Split runs up between threads
        vector<int> statistics(100);
        #pragma omp parallel for num_threads(NUM_THREADS)
        for (int run = 0; run < 100; run++) {
            s = run_single_run(input_dirnames[run], output_filenames[run], l);
            statistics[run] = s;
        }
        output += to_string(l) + ": " + to_string(mean(statistics)) + " +- " + to_string(stdev(statistics)) + "\n";
    }
    ofstream file;
    file.open(output_root);
    file << output << endl;
    file.close();
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
