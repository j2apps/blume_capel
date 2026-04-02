pairs=(
  "3 3.25"
  "3.5 3.5"
  "3.5 3.25"
  "4 3"
  "4 3.5"
  "4 3.25"
)

for pair in "${pairs[@]:1:5}"; do
read ti di <<< "$pair"
# sbatch --job-name="gap_t${ti}d${di}" ./job_scripts/gap_size.sh "./data/sweep/t${ti}_d${di}"
sbatch --job-name="mag_t${ti}d${di}" ./job_scripts/magnet.sh "./data/sweep/t${ti}_d${di}" 
# sbatch --job-name="t${ti}d${di}_con" ./job_scripts/convert_file_format.sh "./data/sweep/t${ti}_d${di}"
# python3 ./corner_contribution/generate_input_lists.py data/sweep/t${ti}_d${di} data/lattice/t${ti}_d${di} 100 100
# sbatch --job-name="sq-t${ti}d${di}" ./job_scripts/square.sh "./data/sweep/t${ti}_d${di}" "./data/lattice/t${ti}_d${di}" 
# bash ./job_scripts/gap_size_analysis.sh "./data/sweep/t${ti}_d${di}"
done




