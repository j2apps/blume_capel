#!/bin/bash
#SBATCH --account=p32813
#SBATCH --error=error.err
#SBATCH --partition=short
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=8
#SBATCH -t 4:00:00
#SBATCH --mem=8M
#SBATCH --job-name="gap7-20"
#SBATCH --output=outlog_gap
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
./compiled/gap_size ./data/ising-7-17/spin ./corner_contribution/data/ising7-17/spin &
./compiled/gap_size ./data/ising-7-17/fk ./corner_contribution/data/ising7-17/fk




