#!/bin/bash
#SBATCH --account=p32813
#SBATCH --array=0-99
#SBATCH --partition=long
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=8
#SBATCH -t 168:00:00
#SBATCH --mem=30M
#SBATCH --job-name=96-I-run
#SBATCH --output=outputs/%x
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load gcc/12.3.0-gcc

run=$((${SLURM_ARRAY_TASK_ID} % 100))
size=$((SLURM_ARRAY_TASK_ID / 100))
declare -a l=(96 0)

./compiled/ilim_${l[$size]} $run ./data/ilim-7-25 0
