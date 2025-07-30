#!/bin/bash
#SBATCH --account=b1140
#SBATCH --array=0-1
#SBATCH --error=error.err
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=8
#SBATCH -t 400:00:00
#SBATCH --mem=50M
#SBATCH --job-name="bigT"
#SBATCH --output=out_%x
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all

declare -a l=(96 128)
./compiled/main_${l[$SLURM_ARRAY_TASK_ID]} 0 ./data/tricritical-7-29 1 0.608 1.966




