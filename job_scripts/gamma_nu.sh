#!/bin/bash
#SBATCH --account=b1140
#SBATCH --error=error.err
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH -t 4:00:00
#SBATCH --mem=10M
#SBATCH --job-name="magnet"
#SBATCH --output=outputs/magnet_batch.txt
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load gcc/12.3.0-gcc

g++ ./corner_contribution/magnet.cpp -std=c++20 -fopenmp -DNUM_THREADS=4 -o ./compiled/magnet

./compiled/magnet $1/spin $1/magnet.txt
