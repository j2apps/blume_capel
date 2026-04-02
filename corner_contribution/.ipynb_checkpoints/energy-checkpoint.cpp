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

class Cluster {
public:
    bool sign;
    vector<int> sites;
};

Cluster generate_cluster_from_string(const string& line) {
    // Convert string to cluster in gap format
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

void get_lattice(vector<int>& lattice, const string& filename) {
    string sample_text;
    ifstream sample_file(filename);
    // Read file line by line
    while (getline(sample_file, sample_text)) {
        Cluster c = generate_cluster_from_string(sample_text);
        if (c.sites.size() == 0) {
            continue;
        }
        // Convert from gap format to position
        int prev = c.sites[0];
        lattice[prev] = c.sign ? 1 : -1;
        for (int i = 1; i < c.sites.size(); i++) {
            int current = prev + c.sites[i];
            prev = current;
            lattice[current] = c.sign ? 1 : -1;
        }
    }
}

double get_energy(const string& filename, int L, int J, int D) {
    // Initialize and fill lattice
    vector<int> lattice(L*L);
    std::fill(lattice.begin(), lattice.end(), 0);
    get_lattice(lattice, filename);

    double e = 0;
    for (int row = 0; row < L; row++) {
        for (int col = 0; col < L; col++) {
            int i = row * L + col;
            int s = lattice[i];

            // single-site term
            e += D * s * s;

            // periodic neighbors
            int right = row * L + (col + 1) % L;
            int down  = ((row + 1) % L) * L + col;

            // nearest-neighbor interaction term
            e += -J * s * lattice[right];
            e += -J * s * lattice[down];
        }
    }
    
    return e;
}

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

array<double, 3> run_single_run(const string& input_dirname, int L, int J, int D) {
    // Initialize gap_size_statistics with size L/2, since there are L/2 possible gap sizes
    double m1 = 0;
    double m2 = 0;
    double e1 = 0;
    double e2 = 0;
    int num_samples = 0;
    // Find all files in the directory, get the gap sizes from each, and update num_samples
    for (const auto & entry : fs::directory_iterator(input_dirname)) {
	    string filepath = entry.path().string();
        const double m = get_magnetization(filepath);
        const double e = get_energy(filepath, L, J, D);
        m1 += abs(m);
        m2 += m*m;
        e1 += e;
        e2 += e*e;
        num_samples ++;
    }
    const double m1_avg = m1/num_samples;
    const double m2_avg = m2/num_samples;
    const double e1_avg = e1/num_samples;
    const double e2_avg = e2/num_samples;
    array<double, 3> result = {m1_avg, m2_avg, e2_avg - e1_avg*e1_avg};
    return result;
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
            return -1;
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

void run_statistics(const string& input_root, const string& output_root, const int n_batches, const int n_runs, const int J, const int D) {
    string output = "batch,L,m,se_m,X,se_X,c,se_c\n";
    for (int l: {16, 24, 32, 48, 64, 96}) {
        // Write string ahead of time to avoid race conditions
	    vector<string> input_dirnames(n_runs);
	    for (int run=0; run<n_runs; run++) {
		    input_dirnames[run] = input_root + "/" + to_string(l) + "/" + to_string(run);
	    }
        // Split runs up between threads
        vector<double> m(n_runs);
        vector<double> x(n_runs);
        vector<double> c(n_runs);
        #pragma omp parallel for num_threads(NUM_THREADS)
        for (int run = 0; run < n_runs; run++) {
            array<double, 3> result = run_single_run(input_dirnames[run], l, J, D);
            m[run] = result[0] / (l*l);
            x[run] = result[1] / (l*l);
            c[run] = result[2] / (l*l);
        }
        // Temporary batched data output
        for (int i = 0; i < n_batches; i++) {
            const int runs_per_batch = n_runs / n_batches;
            vector<double> batched_m(runs_per_batch);
            vector<double> batched_x(runs_per_batch);
            vector<double> batched_c(runs_per_batch);
            for (int j = 0; j < runs_per_batch; j++) {
                batched_m[j] = m[i*runs_per_batch + j];
                batched_x[j] = x[i*runs_per_batch + j];
                batched_c[j] = c[i*runs_per_batch + j];
            }
            output += to_string(i) + ","
            + to_string(l) + ","
            + to_string(mean(batched_m)) + ","
            + to_string(stdev(batched_m)/sqrt(runs_per_batch)) + ","
            + to_string(mean(batched_x)) + ","
            + to_string(stdev(batched_x)/sqrt(runs_per_batch)) + ","
            + to_string(mean(batched_c)) + ","
            + to_string(stdev(batched_c)/sqrt(runs_per_batch)) + "\n";
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
    if (argc != 7) {
        cout << argc << endl;
        return -1;
    }
    const string input = argv[1];
    const string output = argv[2];
    const int n_batches = atoi(argv[3]);
    const int n_runs = atoi(argv[4]);
    const int J = atoi(argv[5]);
    const int D = atoi(argv[6]);
    
    run_statistics(input, output, n_batches, n_runs, J, D);
    return 0;
}
