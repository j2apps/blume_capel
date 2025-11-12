D=1.9659
T=0.6079
i=4
	for j in {1..3};
	do
		d=$(echo "$D + 0.000025*$j" | bc)
		t=$(echo "$T + 0.000025*$i" | bc)
		python3 ./gen_files.py "./data/sweep/t${i}_d${j}"
		bash ./job_scripts/runner.sh "./data/sweep/t${i}_d${j}" $t $d 1 "t${i}_d${j}"
	done
		



