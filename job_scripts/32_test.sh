#!/bin/bash
#SBATCH --account=p32813
#SBATCH --partition=normal
#SBATCH --array=0-2
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -t 08:00:00
#SBATCH --mem=10M
#SBATCH --job-name="32-1-ising"
#SBATCH --output=outlog
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load gcc/12.3.0-gcc
declare -a l=(8 16 32)
./compiled/ising_${l[$SLURM_ARRAY_TASK_ID]}
