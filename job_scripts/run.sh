#!/bin/bash
#SBATCH --account=p32813
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --output=outputs/%x
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
./compiled/main_$1 ${SLURM_ARRAY_TASK_ID} ./data/tri-metro-8-20 0
