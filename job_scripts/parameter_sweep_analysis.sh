pairs=(
  "3 3"
  "3.5 3"
  "2.5 3"
  "3 3.5"
  "3 2.5"
)

for pair in "${pairs[@]}"; do
read ti di <<< "$pair"
# sbatch --job-name="t${ti}d${di}_gap" ./job_scripts/gap_size.sh "./data/sweep/t${ti}_d${di}"
sbatch --job-name="t${ti}d${di}_mag" ./job_scripts/magnet.sh "./data/sweep/t${ti}_d${di}" 
# sbatch --job-name="t${ti}d${di}_con" ./job_scripts/convert_file_format.sh "./data/sweep/t${ti}_d${di}"
# sbatch --job-name="t${ti}d${di}_square" ./job_scripts/square.sh "./data/sweep/t${ti}_d${di}" 
done




