#!/bin/bash
#SBATCH --account=b1140
#SBATCH --error=error.err
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --array=0-9
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -t 48:00:00
#SBATCH --mem=50M
#SBATCH --output=outputs/%x
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge
module load gcc/12.3.0-gcc
g++ -O3 ./corner_contribution/tricritical_sheared.cpp -o ./compiled/square
# python3 
python3 ./corner_contribution/generate_input_lists.py $1 $2 10 100


sizes=( 16 24 32 48 64 96 )
entros=( "spin" )
for entropy in "${entros[@]}"; do
	for size in "${sizes[@]}"; do
        echo "starting L=$size at $(date)"
		./compiled/square $SLURM_ARRAY_TASK_ID $size 0 10 "$1/gap/square/inputs/$entropy/$size/$SLURM_ARRAY_TASK_ID.txt" 123 "$1/gap/square/$entropy"
        echo "finished L=$size at $(date)"
	done
done




