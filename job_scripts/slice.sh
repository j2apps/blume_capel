#!/bin/bash
#SBATCH --account=b1140
#SBATCH --job-name="SLICE"
#SBATCH --error=error.err
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -t 48:00:00
#SBATCH --array=0-24
#SBATCH --mem=50M
#SBATCH --output=outputs/slice_%a.txt
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

# USAGE: sbatch job_scripts/slice.sh [samples per task] [slices per axis]
module purge all
module load gcc/12.3.0-gcc
module load python-anaconda3/2019.10 

samples_per_task=$1
slices_per_axis=$2
first_sample=$(($SLURM_ARRAY_TASK_ID * $samples_per_task))

python3 slice/slice.py \
  "/projects/b1140/mco7887/Potts/samples/Q2" \
  "data/ising3d" \
  "$first_sample" \
  "$samples_per_task" \
  "$slices_per_axis"