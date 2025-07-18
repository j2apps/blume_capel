#!/bin/bash
#SBATCH --account=p32813
#SBATCH --array=0-4
#SBATCH --error=error.err
#SBATCH --partition=short
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=8
#SBATCH -t 00:05:00
#SBATCH --mem=20M
#SBATCH --job-name="128omptest"
#SBATCH --output=omp128log
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all

declare -a threadnums=(1 2 4 6 8)
echo ${threadnums[$SLURM_ARRAY_TASK_ID]}

STARTTIME=$(date +%s)
./compiled/128omp_${threadnums[$SLURM_ARRAY_TASK_ID]} --tasks ${threadnums[$SLURM_ARRAY_TASK_ID]}
ENDTIME=$(date +%s)

echo "Threads: ${threadnums[$SLURM_ARRAY_TASK_ID]} | Time: $((ENDTIME - STARTTIME))"




