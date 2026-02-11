#!/bin/bash
#SBATCH --account=nhb4160
#SBATCH --error=error.err
#SBATCH --partition=short
#SBATCH --nodes=1
#SBATCH --array=0-99
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -t 24:00:00
#SBATCH --mem=20M
#SBATCH --output=outputs/%x
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge
module load gcc/12.3.0-gcc

python3 ./corner_contribution/generate_input_lists.py $1

sizes=( 12 16 24 32 48 64 96 )
entros=( "fk" )
for entropy in "${entros[@]}"; do
	echo "starting $size"
	for size in "${sizes[@]}"; do
		./compiled/square $SLURM_ARRAY_TASK_ID $size 0 20 "$1/gap/square/inputs/$entropy/$size/$SLURM_ARRAY_TASK_ID.txt" 123 "$1/gap/square/$entropy"
	done
done






