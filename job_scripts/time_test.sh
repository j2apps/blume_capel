#!/bin/bash
#SBATCH --account=b1140
#SBATCH --array=0-2
#SBATCH --error=error.err
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=8
#SBATCH -t 00:30:00
#SBATCH --mem=10M
#SBATCH --job-name="128test"
#SBATCH --output=time_test
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all

declare -a l=(64 96 128)

STARTTIME=$(date +%s)
./compiled/test_${l[$SLURM_ARRAY_TASK_ID]} 0 N 1
ENDTIME=$(date +%s)

echo "L: ${l[$SLURM_ARRAY_TASK_ID]} | Time: $((ENDTIME - STARTTIME))"




