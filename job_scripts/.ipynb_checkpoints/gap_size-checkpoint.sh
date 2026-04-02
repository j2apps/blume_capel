#!/bin/bash
#SBATCH --account=p32813
#SBATCH --error=error.err
#SBATCH --partition=short
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH -t 4:00:00
#SBATCH --mem=50M
#SBATCH --output=outputs/gap.txt
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load gcc/12.3.0-gcc
module load python-anaconda3/2019.10 

python3 ./corner_contribution/gen_gss_files.py $1/gap
g++ ./corner_contribution/gap_size_statistics.cpp -std=c++20 -fopenmp -DNUM_THREADS=4 -o ./compiled/gap

# ./compiled/gap $1/spin $1/gap/spin 100
./compiled/gap $1/fk $1/gap/fk 100

echo spin
python3 ./corner_contribution/gap_size_analysis.py $1 100 100






