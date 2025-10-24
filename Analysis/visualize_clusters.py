from matplotlib import pyplot as plt
import numpy as np
import sys

def get_coords(id, L):
    return (id % L, id // L)

if __name__ == "__main__":

    filename = sys.argv[1]
    L = int(sys.argv[2])
    file = open(filename)
    lattice = np.zeros((L,L))

    for line in file.readlines():
        val = 1 if (line[0] == "+") else -1
        cluster = [int(site) for site in line.split(" ")[1:] if site != "\n"]
        if (len(cluster) == 0):
            continue
        sites = list()
        prev = cluster[0]
        sites.append(prev)

        for gap in cluster[1:]:
            curr = prev + gap
            sites.append(curr)
            prev = curr

        for site in cluster:
            lattice[get_coords(site, L)] = val;

    plt.imshow(lattice, cmap='viridis')
    plt.save_fig('im.png')






