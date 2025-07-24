#!/bin/bash
#SBATCH --account=p32813
#SBATCH --array=0-99
#SBATCH --partition=short
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -t 01:00:00
#SBATCH --mem=5M
#SBATCH --job-name="16-ising"
#SBATCH --output=outlog
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load gcc/12.3.0-gcc


./compiled/ising_run_16 $SLURM_ARRAY_TASK_ID ./data/16_burn_test ./data/burn-7-22/ising/16.txt

