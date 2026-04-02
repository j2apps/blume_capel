#!/bin/bash
#SBATCH --account=b1140
#SBATCH --job-name=move
#SBATCH --output=outputs/move_%j.out
#SBATCH --error=outputs/move_%j.err
#SBATCH --time=1:00:00
#SBATCH --cpus-per-task=1
#SBATCH --mem=10M
#SBATCH --partition=b1140
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

for i in {0..9}; do
    rsync -a --ignore-missing-args \
        ./data/ising/fk/64x/$i/{100..999}.txt \
        ./data/ising/fk/64/$i/
done
