#!/bin/bash
#SBATCH --account=b1140
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --output=outputs/%x
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu
module purge all

./compiled/tri-$1 0 $2 1 0.608 1.966 1




