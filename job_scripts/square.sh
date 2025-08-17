#!/bin/bash
#SBATCH --account=p32813
#SBATCH --error=error.err
#SBATCH --partition=short
#SBATCH --array=0-99
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -t 4:00:00
#SBATCH --mem=10M
#SBATCH --job-name="square"
#SBATCH --output=outputs/square
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load python-anaconda3/2019.10 

for l in 12 16 24 32 48 64 96 128; do
    for file in $1/$3/$l/$SLURM_ARRAY_TASK_ID/*.txt; do
        python3 ./corner_contribution/square.py "$file" $l $3 0 $SLURM_ARRAY_TASK_ID $2
  done
done
