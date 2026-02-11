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

def main(root, n_batches):
    # Iterate through each L value
    for cluster_type in ('fk', 'spin'):
        with open(f'{root}/gap/{cluster_type}.txt', 'w') as file:
            file.write('batch,L,corner_contribution,standard_error')
        for l in (12, 16, 24, 32, 48, 64, 96):
            corner_contributions = list()
            # Find all files in the directory
            with os.scandir(f"{root}/gap/{cluster_type}/{l}") as entries:
                for entry in entries:
                    # Get the stats and number of samples from each file
                    gap_size_statistics, num_samples = get_gap_array(entry)
                    if (num_samples==0):
                        continue
                    # Get the corner contribution and append
                    corner_contribution = get_corner_contribution(gap_size_statistics, num_samples, l)
                    corner_contributions.append(corner_contribution)
            # Calculate the mean and SE of the corner contribution

            for i in range(n_batches):
                samples_per_batch = len(corner_contributions) // n_batches
                mean_corner_contribution = statistics.mean(corner_contributions[i*samples_per_batch:(i+1)*samples_per_bacth])
                stdev_corner_contribution = statistics.stdev(corner_contributions[i*samples_per_batch:(i+1)*samples_per_batch])
                with open(f'{root}/gap/{cluster_type}.txt', 'a') as file:
                    file.write(f'\n{i},{l},{mean_corner_contribution},{stdev_corner_contribution/math.sqrt(samples_per_bucket)}')
            print(l, mean_corner_contribution, stdev_corner_contribution/math.sqrt(n_batches))

if __name__ == "__main__":
    root = sys.argv[1]
    n_batches = int(sys.argv[2])
    main(root, n_batches)
    '''
    gap_size_statistics, num_samples = get_gap_array("./sample_data/gap.txt")
    print(gap_size_statistics)
    corner_contribution = get_corner_contribution(gap_size_statistics, num_samples, 64)
    print(corner_contribution)
    '''



