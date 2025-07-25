#!/bin/bash
#SBATCH --account=p32813
#SBATCH --array=0-299
#SBATCH --partition=short
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=2
#SBATCH -t 4:00:00
#SBATCH --mem=10M
#SBATCH --job-name="smallising"
#SBATCH --output=outlog
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load gcc/12.3.0-gcc

run=$((${SLURM_ARRAY_TASK_ID} % 100))
size=$((SLURM_ARRAY_TASK_ID / 100))
declare -a l=(16 32 64)

./compiled/ising_brun${l[$size]} $run ./data/ising-7-24 ./data/burn-7-23/ising/${l[$size]}_burn.txt
