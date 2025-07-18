#!/bin/bash
#SBATCH --account=p32813
#SBATCH --array=0-199
#SBATCH --partition=normal
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH -t 20:00:00
#SBATCH --mem=10M
#SBATCH --job-name="32-16-ising"
#SBATCH --output=32-16-ising-outlog
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load gcc/12.3.0-gcc

run=$((${SLURM_ARRAY_TASK_ID} % 100))
size=$(( 2**( (${SLURM_ARRAY_TASK_ID} / 100) + 4 ) ))

./compiled/ising_4_$size $run ./data/ising-7-17

