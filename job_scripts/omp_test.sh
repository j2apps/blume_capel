#!/bin/bash
#SBATCH --account=p32813
#SBATCH --array=0-4
#SBATCH --error=error.err
#SBATCH --partition=short
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH -t 01:00:00
#SBATCH --mem=10M
#SBATCH --job-name="omp_test"
#SBATCH --output=outlog
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all

declare -a threadnums=(1 2 4 6 8)
echo ${threadnums[$SLURM_ARRAY_TASK_ID]}
./compiled/omptest_${threadnums[$SLURM_ARRAY_TASK_ID]} --tasks ${threadnums[$SLURM_ARRAY_TASK_ID]}




