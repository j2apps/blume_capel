import os
import sys
import numpy as np
ls = (16, 32)
nruns = 100
clustertypes = ('fk', 'spin')

def susceptibility(filename):
    m = 0
    m_sqr = 0
    with open(filename, 'r') as file:
        lines = file.readlines()
        for line in lines:
            pos = 1 if line[0] == '+' else -1
            size = line.strip().split(" ") - 1
            m += size*pos
            m_sqr += size
    return m_sqr - m*m

def batch(dirname, l):
    count = 0
    total_x = 0
    for entry in os.scandir(dirname):
        count += 1
        total_x += susceptibility(entry)
    return total_x / count

def main(input, output_file):
    output = ""
    for type in clustertypes:
        output += f'{type}: \n'
        for l in ls:
            batches = np.zeros(nruns)
            for i in range(nruns):
                dirname = f'{input}/{type}/{l}/{i}'
                batches[i] = batch(dirname, l)
            output += f'{l} {np.mean(batches)} {np.std(batches) / np.sqrt(nruns)}\n'

    print(output)
    with open(output_file, 'w') as file:
        file.write(output)

if __name__ == "__main__":
    input = sys.argv[1]
    output = sys.argv[2]
    main(input, output)