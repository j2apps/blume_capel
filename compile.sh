module purge all
module load gcc/12.3.0-gcc

for l in 8 16;
do
	g++ ./main.cpp -std=c++20 -fopenmp -DL_MACRO=$l -DNUM_THREADS=4 -O3 -o "./compiled/tricrit_"$l
done


