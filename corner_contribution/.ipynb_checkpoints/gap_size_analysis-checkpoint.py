import statistics
import os
import sys
import math
def get_corner_contribution(gap_size_statistics, num_samples, L):
    # Apply corner contribution formula
    acc = 0
    for i in range(0, L//2):
        acc += (i+1) * gap_size_statistics[i]
    return acc / (L*L*num_samples)

def get_gap_array(filename):
    # Get an array of numbers from the text file
    gap_size_statistics = []
    with open(filename, 'r') as file:
        lines = file.readlines()
        num_samples = int(lines[0].strip())
        for line in lines[1].strip().split(" "):
            gap_size_statistics.append(int(line))
    return gap_size_statistics, num_samples

def main(root, n_batches, n_runs):
    # Iterate through each L value
    for cluster_type in ('fk',):
        with open(f'{root}/gap/{cluster_type}.txt', 'w') as file:
            file.write('batch,L,corner_contribution,standard_error')
        for l in (16, 24, 32, 48, 64, 96):
            corner_contributions = list()
            # Find all files in the directory
            for i in range(n_runs):
                entry = f"{root}/gap/{cluster_type}/{l}/{i}.txt"
                gap_size_statistics, num_samples = get_gap_array(entry)
                
                if (num_samples <= 1):
                    continue
                # Get the corner contribution and append
                corner_contribution = get_corner_contribution(gap_size_statistics, num_samples, l)
                corner_contributions.append(corner_contribution)
            # Calculate the mean and SE of the corner contribution
            print(len(corner_contributions))
            for i in range(n_batches):
                print(i)
                samples_per_batch = len(corner_contributions) // n_batches
                mean_corner_contribution = statistics.mean(corner_contributions[i*samples_per_batch:(i+1)*samples_per_batch])
                if samples_per_batch >= 2:
                    stdev_corner_contribution = statistics.stdev(corner_contributions[i*samples_per_batch:(i+1)*samples_per_batch])
                else:
                    stdev_corner_contribution = 0
                with open(f'{root}/gap/{cluster_type}.txt', 'a') as file:
                    file.write(f'\n{i},{l},{mean_corner_contribution},{stdev_corner_contribution/math.sqrt(samples_per_batch)}')
            print(l, mean_corner_contribution, stdev_corner_contribution/math.sqrt(n_batches))

if __name__ == "__main__":
    root = sys.argv[1]
    n_batches = int(sys.argv[2])
    n_runs = int(sys.argv[3])
    main(root, n_batches, n_runs)
    '''
    gap_size_statistics, num_samples = get_gap_array("./sample_data/gap.txt")
    print(gap_size_statistics)
    corner_contribution = get_corner_contribution(gap_size_statistics, num_samples, 64)
    print(corner_contribution)
    '''



