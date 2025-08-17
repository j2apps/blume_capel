#!/bin/bash
#SBATCH --account=b1140
#SBATCH --error=error.err
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=8
#SBATCH -t 1:00:00
#SBATCH --mem=10M
#SBATCH --job-name="gamma_nu"
#SBATCH --output=outputs/gamma_nu
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load gcc/12.3.0-gcc
mkdir -p $2

g++ ./corner_contribution/gamma_nu.cpp -std=c++20 -fopenmp -DNUM_THREADS=8 -o ./compiled/gn

./compiled/gn $1/spin $2/spin.txt




