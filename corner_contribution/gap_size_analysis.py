import numpy
def get_corner_contribtion(gap_size_statistics, num_samples, L, l):
    sum = 0;
    for i in range(1, l+1):
        for j in range(1, L/2 + 1):
            sum += gap_size_statistics[j-1]
    return (1/2) * (1/(L*L)) * (1/num_samples) * sum

