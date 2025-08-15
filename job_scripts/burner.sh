module purge all
module load gcc/12.3.0-gcc

declare -a sizes=(12 24 48)
declare -a cores=(1 2 4)
declare -a partition=(b1140 b1140 b1140)	
declare -a time=(1 1 4)
declare -a mem=(20 30 40)

for i in 0 1 2;
do
	g++ ./main2.cpp -std=c++20 -fopenmp -DL_MACRO=${sizes[i]} -DNUM_THREADS=${cores[i]} -O3 -o "./compiled/main_"${sizes[i]}
done

for i in 0 1 2;
do
	sbatch --cpus-per-task=${cores[i]} --partition=${partition[i]} --time="${time[i]}:00:00" --mem=${mem[i]} --job-name="IBurn_${sizes[i]}" job_scripts/burn.sh ${sizes[i]}
done
