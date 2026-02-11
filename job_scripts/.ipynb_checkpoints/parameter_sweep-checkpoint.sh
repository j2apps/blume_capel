D=1.9659
T=0.6079

pairs=(
  "3 3"
  "3.5 3"
  "2.5 3"
  "3 3.5"
  "3 2.5"
)

for pair in "${pairs[@]}"; do
    read ti di <<< "$pair"
	d=$(bc -l <<< "$D + 0.000025*$di")
	t=$(bc -l <<< "$T + 0.000025*$ti")
	python3 ./gen_files.py "./data/sweep/t${ti}_d${di}"
	bash ./job_scripts/runner.sh "./data/sweep/t${ti}_d${di}" "$t" "$d" 1 "t${ti}_d${di}"
done




