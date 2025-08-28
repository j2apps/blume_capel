module purge all
module load gcc/12.3.0-gcc

size = 8
g++ ./main.cpp -std=c++20 -fopenmp -DL_MACRO=$size -DNUM_THREADS=1 -O3 -o "./compiled/psuedo"
mkdir $1

for i in {-10..10};
do
	t = $i * 0.02 + 0.608
	mkdir $1/$t
	mkdir $1/$t/spin
	mkdir $1/$t/spin/$size
	sbatch --job-name="t-$t" job_scripts/psuedo.sh $size $1/$t/spin/$size t
done
