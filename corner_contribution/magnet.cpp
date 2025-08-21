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

double run_single_run(const string& input_dirname, int L) {
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
    return (m2_avg - m1_avg*m1_avg) / (L*L);
}

double stdev(const std::vector<double>& data) {
    double sum = 0.0;
    double mean, standardDeviation = 0.0;

    for (double value : data) {
        sum += value;
    }
    mean = sum / data.size();

    for (double value : data) {
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

void run_statistics(const string& input_root, const string& output_root) {
    string output = "L X SE\n";

    for (int l: {8, 12, 16, 24, 32, 48, 64}) {
        cout << "Starting " << l << endl;
        int nruns = 100;
        // Write string ahead of time to avoid race conditions
	    vector<string> input_dirnames(nruns);
	    for (int run=0; run<nruns; run++) {
		    input_dirnames[run] = input_root + "/" + to_string(l) + "/" + to_string(run);
	    }

        // Split runs up between threads
        vector<double> data(nruns);
        #pragma omp parallel for num_threads(NUM_THREADS)
        for (int run = 0; run < nruns; run++) {
            data[run] = run_single_run(input_dirnames[run], l);
        }
        output += to_string(l) + " " + to_string(mean(data)) + " " + to_string(stdev(data)/sqrt(nruns)) + "\n";
        cout << "Finished " << l << endl;
    }
    cout << "writing file to: " << output_root << "\n" << flush;
    ofstream file;
    file.open(output_root);
    file << output << endl;
    file.close();
    cout << "wrote to: " << output_root << "\n" << flush;
    
}
int main(int argc, const char * argv[]) {
    // Ensure the correct arguments are in place
    cout << "Starting" << endl;
    if (argc != 3) {
        cout << argc << endl;
        return -1;
    }

    const string input = argv[1];
    const string output = argv[2];
    run_statistics(input, output);
    return 0;
}
