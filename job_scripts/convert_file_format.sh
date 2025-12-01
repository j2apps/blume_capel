#!/bin/bash
#SBATCH --account=b1140
#SBATCH --error=error.err
#SBATCH --partition=b1140
#SBATCH --nodes=1
#SBATCH --array=0-9
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -t 8:00:00
#SBATCH --mem=10M
#SBATCH --job-name="convert"
#SBATCH --output=outputs/convert
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
module load python-anaconda3/2019.10 
nruns=10
run_base=$(( SLURM_ARRAY_TASK_ID * nruns ))
python3 corner_contribution/convert_cluster_to_lattice.py $1 $run_base $nruns

