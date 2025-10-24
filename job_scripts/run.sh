#!/bin/bash
#SBATCH --account=p32813
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --output=outputs/%x
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
# 1:L, 2:file, 3:nsamples, 4:T, 5:D, 6:J, 7:name
./compiled/$7-$1 ${SLURM_ARRAY_TASK_ID} $2 2 $3 $4 $5 $6
