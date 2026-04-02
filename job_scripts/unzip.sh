#!/bin/bash
#SBATCH --account=b1140
#SBATCH --partition=b1140
#SBATCH --time=24:00:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --job-name=unzip
#SBATCH --output=unzip.out
#SBATCH --mem=30M
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

tar -xzf "data/sweep_archives/$1.tar.gz" -C "data/sweep"


