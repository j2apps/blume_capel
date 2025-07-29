#!/bin/bash
#SBATCH --account=b1140
#SBATCH --array=0-2
#SBATCH --error=error.err
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH -t 16:00:00
#SBATCH --mem=50M
#SBATCH --job-name="smI"
#SBATCH --output=out_%x
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all

declare -a l=(16 32 64)
./compiled/ilim_${l[$SLURM_ARRAY_TASK_ID]} 0 ./data/ilim-7-25 1




