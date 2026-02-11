#!/bin/bash
#SBATCH --account=b1140
#SBATCH --job-name="square_test"
#SBATCH --partition=b1140
#SBATCH --mem=10m
#SBATCH --time=02:00:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --output=outputs/%x
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

source ~/.bashrc
conda activate bc-3-10

start=$(date +%s)

for sample in {0..100}; do
	python ./corner_contribution/square.py ./data/tc-10-8/fk/16/50/${sample}.txt 16 type 0 ./data/tc-10-8/square_test.txt
done

end=$(date +%s)
runtime=$((end - start))
echo "Runtime: ${runtime} seconds"





