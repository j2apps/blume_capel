import os
import sys
import numpy as np

# TODO
# change convert_file() to use output_lattice_format() 
# use form_clusters to generate random, fk-like clusters
@dataclass
class Cluster:
    value: int
    sites: List[Tuple[int,int]]

def form_clusters(lattice: np.ndarray, bond_prob: float) -> List[Cluster]:
    L = lattice.shape[0]

    visited = np.zeros_like(lattice, dtype=bool)
    clusters: List[Cluster] = []

    neighbors = [(1,0), (-1,0), (0,1), (0,-1)]

    for y in range(L):
        for x in range(L):

            if visited[y, x]:
                continue

            value = lattice[y, x]

            stack = [(x, y)]
            visited[y, x] = True

            cluster = Cluster(sign=(value == 1), sites=[(x, y)])

            while stack:
                cx, cy = stack.pop()

                for dx, dy in neighbors:
                    nx = (cx + dx) % L
                    ny = (cy + dy) % L

                    if (not visited[ny, nx]
                        and lattice[ny, nx] == value
                        and random.random() < bond_prob):

                        visited[ny, nx] = True
                        stack.append((nx, ny))
                        cluster.sites.append((nx, ny))

            clusters.append(cluster)
    return clusters

def get_lattice(clusters, L):
    lattice = np.full((L,L), 0)
    for cluster in clusters:
        for x, y in cluster.sites:
            lattice[x,y] = cluster.value
    return lattice

def extract_clusters(filename, L):
    clusters = list()
    with open(filename, 'r') as file:
        for line in file:
            split = line.strip().split()
            if len(split) == 0:
                continue
            curr_posn = int(split[1])
            x = curr_posn % L
            y = curr_posn // L
            sign = -1 if split[0] == '-' else 1
            print(split[0], sign)
            cluster = Cluster(value=sign, sites=[(x,y)])
            gaps = split[2:]
            for gap in gaps:
                curr_posn += int(gap)
                x = curr_posn % L
                y = curr_posn // L
                cluster.sites.append(x,y)
    return clusters

def convert_file(filename, L):
    # Get spin clusters from data
    spin_clusters = extract_clusters(filename, L)
    spin_lines = output_lattice_format(spin_clusters, L)

    # Get random clusters from clustering algorithm
    lattice = get_lattice(clusters, L)
    rand_clusters = form_clusters(lattice, P_RAND)
    rand_lines = output_lattice_format(rand_clusters, L)

def list_files_scandir(path):
    files = [entry.name for entry in os.scandir(path) if entry.is_file()]
    return files

def convert_directory(input_dirname, output_dirname, L):
    files = list_files_scandir(input_dirname)
    for filename in files:
        in_path = os.path.join(input_dirname, filename)
        spin_lines, rand_lines = convert_file(in_path, L)
        
        with open(f"{output_dirname}/{filename}", 'w') as file:
            file.write(convert_file(in_path, L))
    
# TODO: CONFIGURE OUTPUTS TO WORK WITH RAND
# TODO: Think about making this a seperate file that configures the FK clusters, 
# since this one was originally just for converting to lattice format
def convert_dataset(input_root, output_root, sizes, runs):
    os.makedirs(f"{output_root}/{type}", exist_ok=True)
    for L in sizes:
        os.makedirs(f"{output_root}/{type}/{L}", exist_ok=True)
        for run in runs:
            os.makedirs(f"{output_root}/{type}/{L}/{run}", exist_ok=True)
            input_dirname = f"{input_root}/{type}/{L}/{run}"
            output_dirname = f"{output_root}/{type}/{L}/{run}"
            convert_directory(input_dirname, output_dirname, L)

if __name__ == "__main__":
    # For converting entire dataset at once
    input_root = sys.argv[1]
    run_base = int(sys.argv[2])
    nruns = int(sys.argv[3])
    output_root = sys.argv[4]
    os.makedirs(output_root, exist_ok=True)
    sizes = (16, 24, 32, 48, 64, 96)
    runs = range(run_base, run_base+nruns)
    convert_dataset(input_root, output_root, sizes, runs)
