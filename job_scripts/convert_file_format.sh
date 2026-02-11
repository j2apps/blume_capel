#!/bin/bash
#SBATCH --account=b1140
#SBATCH --error=error.err
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --array=0-19
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -t 8:00:00
#SBATCH --mem=10M
#SBATCH --output=outputs/convert%a.txt
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load python-anaconda3/2019.10 

n_per_run=5
run_base=$(( SLURM_ARRAY_TASK_ID * n_per_run ))
sizes=( 12 16 24 32 48 64 96 )
entros=( "spin" "fk" )
for entropy in "${entros[@]}"; do
	for size in "${sizes[@]}"; do
		echo "starting $size"
		for ((n=run_base; n<run_base+n_per_run; n++)); do
		    python3 ./corner_contribution/convert_cluster_to_lattice.py \
			"$1/$entropy/$size/$n" \
			"$1/$entropy/$size/$n/lattice" \
			"$size"
		done

	done
done


