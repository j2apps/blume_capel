#!/bin/bash
#SBATCH --account=p32813
#SBATCH --error=error.err
#SBATCH --partition=short
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=10
#SBATCH -t 4:00:00
#SBATCH --mem=8M
#SBATCH --job-name="gap2"
#SBATCH --output=outlog
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

module purge all
./compiled/gap_size ../blume_capel_old/clusters_ising_2/spin ./corner_contribution/data/ising/spin &
./compiled/gap_size ../blume_capel_old/clusters_ising_2/fk ./corner_contribution/data/ising/fk




