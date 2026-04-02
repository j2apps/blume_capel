#!/bin/bash
#SBATCH --account=b1140
#SBATCH --partition=b1140
#SBATCH --time=24:00:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --job-name=lattice_del
#SBATCH --output=delete.out
#SBATCH --mem=25M
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

entropies=( "fk" )
sizes=(16 24 32 48 64 96)
for entropy in "${entropies[@]}"; do
for size in "${sizes[@]}"; do
    path="$1/$entropy/$size"
    rm -rf "$path"
done
done


