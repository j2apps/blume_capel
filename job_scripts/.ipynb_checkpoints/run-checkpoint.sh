#!/bin/bash
#SBATCH --account=p32813
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --output=outputs/%x
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module load python-anaconda3/2019.10 
# 1:L, 2:root directory, 3:nsamples, 4:T, 5:D, 6:J, 7:name

./compiled/$7-$1 ${SLURM_ARRAY_TASK_ID} $2 0 $3 $4 $5 $6
# python3 ./corner_contribution/convert_cluster_to_lattice.py ./$2/fk/$1/${SLURM_ARRAY_TASK_ID} ./$2/fk/$1/${SLURM_ARRAY_TASK_ID}/lattice $1
# python3 ./corner_contribution/convert_cluster_to_lattice.py ./$2/spin/$1/${SLURM_ARRAY_TASK_ID} ./$2/spin/$1/${SLURM_ARRAY_TASK_ID}/lattice $1


