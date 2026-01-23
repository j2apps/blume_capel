#!/bin/bash
#SBATCH --account=b1140
#SBATCH --error=error.err
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --array=0-9
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -t 24:00:00
#SBATCH --mem=20M
#SBATCH --job-name="square"
#SBATCH --output=outputs/square%a.txt
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge
module load gcc/12.3.0-gcc

python3 ./corner_contribution/generate_input_lists.py $1
sizes=( 16 24 32 48 64 )
entros=( "spin" "fk" )
for entropy in "${entros[@]}"; do
echo "starting $2"
./compiled/square $SLURM_ARRAY_TASK_ID $2 0 20 "$1/gap/square/inputs/$entropy/$2/$SLURM_ARRAY_TASK_ID.txt" 123 "$1/gap/square/$entropy"
echo "finished $2"
done






