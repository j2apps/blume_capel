di=3
for ti in 1 3;
do
sbatch ./job_scripts/gap_size.sh "./data/sweep/t${ti}_d${di}"
sbatch --job-name="magnet_t${ti}_d${di}" ./job_scripts/gamma_nu.sh "./data/sweep/t${ti}_d${di}" 
done	



