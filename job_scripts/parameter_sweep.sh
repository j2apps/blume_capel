D=1.96590
T=0.60790
# D=1.96581491975
# T=0.60857757224
pairs=(
    "3 4"
    "2.5 3.5"
    "3.5 3.5"
)

for pair in "${pairs[@]}"; do
    read ti di <<< "$pair"
	d=$(bc -l <<< "$D + 0.000025*$di")
	t=$(bc -l <<< "$T + 0.000025*$ti")
	python3 ./gen_files.py "./data/sweep/t${ti}_d${di}"
	bash ./job_scripts/runner.sh "./data/sweep/t${ti}_d${di}" "$t" "$d" 1 "t${ti}_d${di}"
done




