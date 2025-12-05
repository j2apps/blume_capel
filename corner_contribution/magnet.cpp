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
#include <cmath>
#include <utility>

using namespace std;
namespace fs = std::filesystem;

#ifndef NUM_THREADS
#define NUM_THREADS 1
#endif

double get_magnetization(const string& filename) {
    ifstream sample_file(filename);
    if (sample_file.is_open()) {
        // File opened successfully, proceed with reading
    } else {
        cerr << "Error: Could not open file." << std::endl;
    }
    string sample_text;
    double m = 0;
    // Use a while loop together with the getline() function to read the file line by line
    while (getline(sample_file, sample_text)) {
        int sign;
        if (sample_text[0] == '+') {
            sign = 1;
        }
        else if (sample_text[0] == '-') {
            sign = -1;
        }
	    else {
            continue;
        }
		
        stringstream ss(sample_text);
        string segment;
	    int count = -1;
        while (getline(ss, segment, ' ')) {
            count ++;
        }
        m += sign*count;
    }
    return m;
}

pair<double, double> run_single_run(const string& input_dirname, int L) {
    // Initialize gap_size_statistics with size L/2, since there are L/2 possible gap sizes
    double m1 = 0;
    double m2 = 0;
    int num_samples = 0;
    // Find all files in the directory, get the gap sizes from each, and update num_samples
    for (const auto & entry : fs::directory_iterator(input_dirname)) {
	    string filepath = entry.path().string();
        const double m = get_magnetization(filepath);
        m1 += abs(m);
        m2 += m*m;
        num_samples ++;
    }
    const double m1_avg = m1/num_samples;
    const double m2_avg = m2/num_samples;
    pair<double, double> result = make_pair(m1_avg, m2_avg);
    return result;
    //return (m2_avg - m1_avg*m1_avg) / (L*L);
}

double stdev(const std::vector<double>& data) {
    double sum = 0.0;
    double standardDeviation = 0.0;

    for (const double value : data) {
        sum += value;
    }
    const double mean = sum / data.size();

    for (const double value : data) {
        standardDeviation += (value - mean) * (value - mean);
    }

    return sqrt(standardDeviation / data.size());
}

double mean(const std::vector<double>& data) {
    double sum = 0.0;
    for (double value : data) {
        sum += value;
    }
    return sum / data.size();
}

int countSubdirectories(const fs::path& directoryPath) {
    int count = 0;
    try {
        if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
            cerr << "Error: Path does not exist or is not a directory." << endl;
            return -1; // Indicate an error
        }

	for (const auto& entry : fs::directory_iterator(directoryPath)) {
            bool has_files = false;
            for (const auto & entry : fs::directory_iterator(directoryPath)) {
                has_files = true;
                break;
            }
            if (fs::is_directory(entry.status()) && has_files) {
                count++;
            }
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
        return -1; // Indicate an error
    }
    return count;
}

void run_statistics(const string& input_root, const string& output_root, const int n_batches) {
    string output = "batch,L,m,se_m,X,se_X\n";
    for (int l: {8, 12, 16, 24, 32, 48, 64}) {
        int nruns = 10;
        // Write string ahead of time to avoid race conditions
	    vector<string> input_dirnames(nruns);
	    for (int run=0; run<nruns; run++) {
		    input_dirnames[run] = input_root + "/" + to_string(l) + "/" + to_string(run);
	    }
        // Split runs up between threads
        vector<double> m(nruns);
        vector<double> x(nruns);
        #pragma omp parallel for num_threads(NUM_THREADS)
        for (int run = 0; run < nruns; run++) {
            pair<double, double> result = run_single_run(input_dirnames[run], l);
            m[run] = result.first / (l*l);
            x[run] = (result.second - result.first*result.first) / (l*l);
        }
        // Temporary batched data output
        for (int i = 0; i < n_batches; i++) {
            const int runs_per_batch = nruns / n_batches;
            vector<double> batched_m(runs_per_batch);
            vector<double> batched_x(runs_per_batch);
            for (int j = 0; j < runs_per_batch; j++) {
                batched_m[j] = m[i*runs_per_batch + j];
                batched_x[j] = x[i*runs_per_batch + j];
            }
            output += to_string(i) + ","
            + to_string(l) + ","
            + to_string(mean(batched_m)) + ","
            + to_string(stdev(batched_m)/sqrt(n_batches)) + ","
            + to_string(mean(batched_x)) + ","
            + to_string(stdev(batched_x)/sqrt(n_batches)) + "\n";
        }
	    ofstream file;
        file.open(output_root);
        file << output << endl;
        file.close();
    }
}
int main(int argc, const char * argv[]) {
    // Ensure the correct arguments are in place
    //cout << "Starting" << endl;
    if (argc != 4) {
        cout << argc << endl;
        return -1;
    }
    const string input = argv[1];
    const string output = argv[2];
    //const int n_batches = atoi(argv[3]);
    const int n_batches = 5;
    
    run_statistics(input, output, n_batches);
    return 0;
}
