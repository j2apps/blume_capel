#!/bin/bash
#SBATCH --account=b1140
#SBATCH --partition=b1140
#SBATCH --time=24:00:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --job-name=del_dir
#SBATCH --output=delete.out
#SBATCH --mem=10M
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

rm -rf $1


