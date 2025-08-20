module purge all
module load gcc/12.3.0-gcc

declare -a sizes=(12 16 24 32)
declare -a cores=(1 1 2 2)
declare -a partition=(short short short short)	
declare -a time=(1 1 4 4)
declare -a mem=(20 20 20 20)

for i in 0 1 2 3;
do
	g++ ./main.cpp -std=c++20 -fopenmp -DL_MACRO=${sizes[i]} -DNUM_THREADS=${cores[i]} -O3 -o "./compiled/main_"${sizes[i]}
done

for i in 0 1 2 3;
do
	sbatch --cpus-per-task=${cores[i]} --partition=${partition[i]} --time="${time[i]}:00:00" --mem=${mem[i]} --job-name="TRun_${sizes[i]}" --array="0-$1" job_scripts/run.sh ${sizes[i]}
done
