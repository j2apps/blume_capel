import statistics
import os
import sys
def get_corner_contribution(gap_size_statistics, num_samples, L, l):
    # Apply corner contribution formula
    acc = 0
    for i in range(0, l):
        for j in range(i, L//2):
            acc += gap_size_statistics[j]
    return (1/(L*L)) * (1/num_samples) * acc

def get_gap_array(filename):
    # Get an array of numbers from the text file
    gap_size_statistics = []
    with open(filename, 'r') as file:
        lines = file.readlines()
        num_samples = int(lines[0].strip())
        for line in lines[1].strip().split(" "):
            gap_size_statistics.append(int(line))
    return gap_size_statistics, num_samples


if __name__ == "__main__":
    root = sys.argv[1]
    # Iterate through each L value
    for cluster_type in ('fk', 'spin'):
        with open(f'{root}/gap/{cluster_type}.txt', 'w') as file:
            file.write('L N SE')
        for l in (12, 16, 24, 32, 48, 64, 96, 128):
            corner_contributions = list()
            # Find all files in the directory
            with os.scandir(f"{root}/gap/{cluster_type}/{l}") as entries:
                for entry in entries:
                    # Get the stats and number of samples from each file
                    gap_size_statistics, num_samples = get_gap_array(entry)
                    if (num_samples==0):
                        continue
                    # Get the corner contribution and append
                    corner_contribution = get_corner_contribution(gap_size_statistics, num_samples, l, l//2)
                    corner_contributions.append(corner_contribution)
            # Calculate the mean and SE of the corner contribution
            mean_corner_contribution = statistics.mean(corner_contributions)
            stdev_corner_contribution = statistics.stdev(corner_contributions)
            with open(f'{root}/gap/{cluster_type}.txt', 'a') as file:
                file.write(f'\n{l} {mean_corner_contribution} {stdev_corner_contribution/10}')
            print(l, mean_corner_contribution, stdev_corner_contribution/10)


'''
FK
8 0.2783826612119698 0.002303475310839541
16 0.16256559895833333 0.00109803774683958
32 0.1043165234375 0.0002353361176152799
64 0.07734922607421875 0.00011746424799293661
128 0.06287702882456911 0.00015511300804922631

SPIN 
8 1.3505504166666666 0.0052778585021737235
16 1.9850557291666666 0.008406295226384416
32 2.7647586979166667 0.004953275554442441
64 4.237573526312319 0.004891779490651527
128 6.983371966802 0.015233393727754969
'''

'''
7/20
Spin
16 1.1279961197916666 0.003380203054046109
32 1.2086592903645832 0.004896044570192812
64 1.290726540038491 0.008930008161243666

FK
16 1.0841324739583333 0.0019061235986048727
32 1.1972618684895833 0.003399348718826886
64 1.315605924956948 0.008110064361817281


'''


