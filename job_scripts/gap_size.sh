#!/bin/bash
#SBATCH --account=b1140
#SBATCH --error=error.err
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=4
#SBATCH -t 2:00:00
#SBATCH --mem=10M
#SBATCH --job-name="gap"
#SBATCH --output=outputs/gap
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load gcc/12.3.0-gcc

g++ ./corner_contribution/gap_size_statistics.cpp -std=c++20 -fopenmp -DNUM_THREADS=4 -o ./compiled/gap
python3 ./corner_contribution/gen_gss_files.py ./corner_contribution/data/tricritical-7-29

./compiled/gap ./data/tricritical-7-29/spin ./corner_contribution/data/tricritical-7-29/spin &
./compiled/gap ./data/tricritical-7-29/fk ./corner_contribution/data/tricritical-7-29/fk




