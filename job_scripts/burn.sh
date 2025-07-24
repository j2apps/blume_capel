#!/bin/bash
#SBATCH --account=b1140
#SBATCH --array=0-1
#SBATCH --error=error.err
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=2
#SBATCH -t 48:00:00
#SBATCH --mem=10M
#SBATCH --job-name="medbc"
#SBATCH --output=output
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all

declare -a l=(32 64)
./compiled/bc_run_${l[$SLURM_ARRAY_TASK_ID]} 0 ./data/burn-7-23/bc NONE




