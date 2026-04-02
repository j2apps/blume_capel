import os
import sys
import numpy as np

def convert_file(filename, L):
    lattice = np.full(L*L, -1, dtype=int)
    with open(filename, 'r') as file:
        curr_id = 0
        # Assign ids to clusters
        for line in file:
            split = line.strip().split()
            if len(split) == 0:
                continue
            curr_posn = int(split[1])
            lattice[curr_posn] = curr_id
            gaps = split[2:]
            for gap in gaps:
                curr_posn += int(gap)
                lattice[curr_posn] = curr_id
            curr_id += 1
        # Give unique ids to empty sites
        mask = (lattice == -1)
        # n_empty = mask.sum()
        # lattice[mask] = np.arange(curr_id, curr_id + n_empty)
    # Convert lattice to str
    rows = []
    for i in range(L):
        row = lattice[i*L:(i+1)*L]
        rows.append(" ".join(str(x) for x in row))
    output_str = "\n".join(rows)
    return output_str

def list_files_scandir(path):
    files = [entry.name for entry in os.scandir(path) if entry.is_file()]
    return files

def convert_directory(input_dirname, output_dirname, L):
    print(input_dirname)
    files = list_files_scandir(input_dirname)
    for filename in files:
        print(filename)
        in_path = os.path.join(input_dirname, filename)
        out_path = os.path.join(output_dirname, filename)
        if os.path.exists(out_path) and os.path.getsize(out_path) > 0:
            continue
        with open(out_path, "w") as file:
            file.write(convert_file(in_path, L))
    

def convert_dataset(input_root, output_root, types, sizes, runs):
    for type in types:
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
    output_root = f"{input_root}/lattice"
    os.makedirs(output_root, exist_ok=True)
    sizes = (16, 24, 32, 48, 64, 96)
    types = ("spin", "fk")
    runs = range(run_base, run_base+nruns)
    convert_dataset(input_root, output_root, types, sizes, runs)
    # print(convert_file(input_root, 8))
    
    # For converting individual runs
    '''
    input_dir = sys.argv[1]
    output_dir = sys.argv[2]
    L = int(sys.argv[3])
    os.makedirs(output_dir, exist_ok=True)
    print(output_dir)
    convert_directory(input_dir, output_dir, L)
    '''

