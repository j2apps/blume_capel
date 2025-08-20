import os
import sys
import numpy as np
ls = (12, 16, 24, 32, 48, 64, 96, 128)
nruns = 100
clustertypes = ('fk', 'spin')

def magnetization(filename, l):
    m = 0
    with open(filename, 'r') as file:
        lines = file.readlines()
        for line in lines:
            pos = 1 if line[0] == '+' else -1
            size = len(line.strip().split(" ")) - 1
            m += size*pos
    return m/(l*l)

def batch(dirname, l):
    count = 0
    m1 = 0
    m2 = 0
    for entry in os.scandir(dirname):
        count += 1
        m = magnetization(entry, l)
        m1 += np.abs(m)
        m2 += m*m
    m1_avg = m1/count
    m2_avg = m2/count
    return (m2_avg - m1_avg*m1_avg)

def main(input, output_file):
    with open(output_file, 'w') as file:
        	    file.write("Starting\n")
    for type in clustertypes:
        with open(output_file, 'a') as file:
                file.write(f"{type}:\n")
        for l in ls:
            batches = np.zeros(nruns)
            for i in range(nruns):
                dirname = f'{input}/{type}/{l}/{i}'
                batches[i] = batch(dirname, l)
            with open(output_file, 'a') as file:
                file.write(f"{l} {np.mean(batches)} {np.std(batches) / np.sqrt(nruns)}\n")
    

if __name__ == "__main__":
    input = sys.argv[1]
    output = sys.argv[2]
    main(input, output)
