import statistics
import os
def get_corner_contribtion(gap_size_statistics, num_samples, L, l):
    acc = 0
    for i in range(1, l+1):
        for j in range(1, L/2 + 1):
            acc += gap_size_statistics[j-1]
    return (1/2) * (1/(L*L)) * (1/num_samples) * acc

def get_gap_array(filename):
    gap_size_statistics = []
    with open(filename, 'r') as file:
        lines = file.readlines()
        num_samples = lines[0]
        for i in range(1, len(lines)):
            gap_size_statistics.append(int(lines[i]))
    return gap_size_statistics, num_samples


if __name__ == "__main__":
    root = "./"

    for l in (8, 16, 32, 64, 128):
        corner_contributions = list()
        with os.scandir(f"{root}/{l}") as entries:
            for entry in entries:
                gap_size_statistics, num_samples = get_gap_array(entry)
                corner_contribution = get_corner_contribtion(gap_size_statistics, num_samples, l, l/2)
                corner_contributions.append(corner_contribution)
        mean_corner_contribution = statistics.mean(corner_contributions)
        stdev_corner_contribution = statistics.stdev(corner_contributions)
        print(l, mean_corner_contribution, stdev_corner_contribution)






