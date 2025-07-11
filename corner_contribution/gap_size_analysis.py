import statistics
import os
def get_corner_contribution(gap_size_statistics, num_samples, L, l):
    # Apply corner contribution formula
    acc = 0
    for i in range(0, l):
        for j in range(i, L//2):
            acc += gap_size_statistics[j]
    return (1/2) * (1/(L*L)) * (1/num_samples) * acc

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
    root = "./corner_contribution/data/ising/spin"
    # Iterate through each L value
    for l in (8, 16, 32, 64, 128):
        corner_contributions = list()
        # Find all files in the directory
        with os.scandir(f"{root}/{l}") as entries:
            for entry in entries:
                # Get the stats and number of samples from each file
                gap_size_statistics, num_samples = get_gap_array(entry)
                # Get the corner contribution and append
                corner_contribution = get_corner_contribution(gap_size_statistics, num_samples, l, l//2)
                corner_contributions.append(corner_contribution)
        # Calculate the mean and SE of the corner contribution
        mean_corner_contribution = statistics.mean(corner_contributions)
        stdev_corner_contribution = statistics.stdev(corner_contributions)
        print(l, mean_corner_contribution, stdev_corner_contribution)


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



