#!/bin/bash
#SBATCH --account=p32813
#SBATCH --partition=short
#SBATCH --array=0-1
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -t 08:00:00
#SBATCH --mem=10M
#SBATCH --output=outputs/%x
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all

./compiled/psuedo ${SLURM_ARRAY_TASK_ID} $2 0 $3 1.966 1
