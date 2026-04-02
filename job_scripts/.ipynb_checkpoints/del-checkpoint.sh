#!/bin/bash
#SBATCH --account=b1140
#SBATCH --partition=b1140
#SBATCH --time=24:00:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --job-name=lattice_del
#SBATCH --output=delete.out
#SBATCH --mem=10M
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

entropies=("fk" "spin")
sizes=(8 12 16 24 32 64 96)
for entropy in "${entropies[@]}"; do
for size in "${sizes[@]}"; do
for r in {0..99}; do
    path="./data/sweep/$1/$entropy/$size/$r/lattice"

    [[ -d "$path" ]] || continue   # skip if missing
    [[ -n "$1" && "$1" != "/" ]] || exit 1  # safety guard

    rm -rf "$path"
done
done
done


