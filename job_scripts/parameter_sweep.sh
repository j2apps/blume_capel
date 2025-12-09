D=1.9659
T=0.6079

di=3.5
ti=1

d=$(echo "$D + 0.000025*$di" | bc)
t=$(echo "$T + 0.000025*$ti" | bc)
python3 ./gen_files.py "./data/sweep/t${ti}_d${di}"
bash ./job_scripts/runner.sh "./data/sweep/t${ti}_d${di}" $t $d 1 "t${ti}_d${di}"



