module purge all
module load gcc/12.3.0-gcc

for l in 16 32 64;
do
	g++ ./main.cpp -std=c++20 -fopenmp -DL_MACRO=$l -DNUM_THREADS=2 -O3 -o "./compiled/ising_brun"$l
done


