module purge all
module load gcc/12.3.0-gcc

declare -a sizes=(16 32 64)
declare -a cores=(1 2 4)
declare -a partition=(short normal long)	
declare -a time=(2 24 168)
declare -a mem=(20 20 30)

for i in $(seq 0 2);
do
	g++ ./main.cpp -std=c++20 -fopenmp -DL_MACRO=${sizes[i]} -DNUM_THREADS=${cores[i]} -O3 -o "./compiled/main_"${sizes[i]}
done

for i in $(seq 0 2);
do
	sbatch --cpus-per-task=${cores[i]} --partition=${partition[i]} --time="${time[i]}:00:00" --mem=${mem[i]} --job-name="TRun_${sizes[i]}" --array="0-$1" job_scripts/run.sh ${sizes[i]}
done
