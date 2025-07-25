#!/bin/bash
#SBATCH --account=b1140
#SBATCH --array=0-50
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=2
#SBATCH -t 2:00:00
#SBATCH --mem=10M
#SBATCH --job-name="smallisingburn"
#SBATCH --output=outlog
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all-7
module load gcc/12.3.0-gcc

run=$((${SLURM_ARRAY_TASK_ID} % 25))
size=$((SLURM_ARRAY_TASK_ID / 25))
declare -a l=(16 32)

./compiled/ising_run_${l[$size]} $run ./data/ising-7-25
