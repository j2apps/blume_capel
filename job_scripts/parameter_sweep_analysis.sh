i=4
for j in {1..3};
do
	sbatch ./job_scripts/gap_size.sh "./data/sweep/t${i}_d${j}"
	sbatch ./job_scripts/gamma_nu.sh "./data/sweep/t${i}_d${j}"
done
		



