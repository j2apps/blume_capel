import os
import sys
import numpy as np

def convert_file(filename, L):
    lattice = np.full(L*L, -1, dtype=int)
    with open(filename, 'r') as file:
        curr_id = L*L
        # Assign ids to clusters
        for line in file:
            split = line.split()
            curr_posn = int(split[1])
            lattice[curr_posn] = curr_id
            gaps = split[2:]
            for gap in gaps:
                curr_posn += int(gap)
                lattice[curr_posn] = curr_id
            curr_id += 1
        # Give unique ids to empty sites
        mask = (lattice == -1)
        n_empty = mask.sum()
        lattice[mask] = np.arange(curr_id, curr_id + n_empty)
    # Convert lattice to str
    rows = []
    for i in range(L):
        row = lattice[i*L:(i+1)*L]
        rows.append(" ".join(str(x) for x in row))
    output_str = "\n".join(rows)
    return output_str

def convert_directory(input_dirname, output_dirname, L):
    files = os.listdir(input_dirname)
    for i, filename in files:
        with open(f"{output_dirname}/{i}") as file:
            file.write(convert_file(filename, L))

def convert_dataset(input_root, output_root, types, sizes):
    for type in types:
        os.mkdir(f"{output_root}/{type}", exist_ok=True)
        for L in sizes:
            os.mkdir(f"{output_root}/{type}/{L}", exist_ok=True)
            input_dirname = f"{input_root}/{type}/{L}"
            output_dirname = f"{output_root}/{type}/{L}"
            convert_directory(input_dirname, output_dirname, L)

if __name__ == "__main__":
    input_root = sys.argv[1]
    output_root = sys.argv[2]
    sizes = (8, 12, 16, 24, 32, 64)
    types = ("fk", "spin")
    convert_dataset(input_root, output_root)