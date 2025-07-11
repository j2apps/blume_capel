#!/bin/bash
#SBATCH --account=p32813
#SBATCH --array=0-499
#SBATCH --partition=long
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH -t 168:00:00
#SBATCH --mem=10M
#SBATCH --job-name="blume-capel-r1"
#SBATCH --output=outlog
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load gcc/12.3.0-gcc

run=$((${SLURM_ARRAY_TASK_ID} % 100))
size=$(( 2**( (${SLURM_ARRAY_TASK_ID} / 100) + 3 ) ))

./compiled/bc_$size $run 

