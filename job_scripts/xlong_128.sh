#!/bin/bash
#SBATCH --account=p32813
#SBATCH --array=0-99
#SBATCH --partition=xlong
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH -t 500:00:00
#SBATCH --mem=1G
#SBATCH --job-name="xlong_128_ising_try2"
#SBATCH --output=outlog
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load gcc/12.3.0-gcc

run=$((${SLURM_ARRAY_TASK_ID}))
size=$((128))

./ising$size $run

