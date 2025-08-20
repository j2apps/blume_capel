#!/bin/bash
#SBATCH --account=b1140
#SBATCH --error=error.err
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -t 2:00:00
#SBATCH --mem=10M
#SBATCH --job-name=magnet
#SBATCH --output=outputs/magnet
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load python-anaconda3/2019.10

python ./corner_contribution/magnetization.py $1 $2




