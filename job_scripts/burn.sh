#!/bin/bash
#SBATCH --account=p32813
#SBATCH --array=0-3
#SBATCH --error=error.err
#SBATCH --partition=normal
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH -t 48:00:00
#SBATCH --mem=10M
#SBATCH --job-name="64bcb"
#SBATCH --output=output
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all

declare -a l=(8 16 32 64)
./compiled/bc_burn_${l[$SLURM_ARRAY_TASK_ID]} 0 ./data/burn-7-22/bc




