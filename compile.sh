module purge all
module load gcc/12.3.0-gcc

for l in 8 16 31 64 128;
do
	g++ ./main.cpp -std=c++20 -fopenmp -DL_MACRO=$l -DNUM_THREADS=4 -o "./compiled/bc_"$l
done


