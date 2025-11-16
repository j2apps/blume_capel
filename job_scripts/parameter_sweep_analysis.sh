ti=1
for di in 2 3 4;
do
	sbatch ./job_scripts/gap_size.sh "./data/sweep/t${ti}_d${di}"
	sbatch ./job_scripts/gamma_nu.sh "./data/sweep/t${ti}_d${di}"
done
		



