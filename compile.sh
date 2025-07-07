module purge all
module load gcc/12.3.0-gcc

for n in 1 2 4 6 8;
do
	g++ ./mainomp.cpp -std=c++20 -fopenmp -DL_MACRO=32 -DNUM_THREADS=$n -o "./compiled/omptest_"$n
done


