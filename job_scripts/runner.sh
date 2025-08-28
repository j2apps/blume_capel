module purge all
module load gcc/12.3.0-gcc

declare -a sizes=(8 12 16 24 32)
declare -a cores=(1 1 1 2 4)
declare -a partition=(short short short short short)	
declare -a time=(1 1 1 4 4)
declare -a mem=(20 20 20 20 20)

for i in {0..3};
do
	g++ ./main.cpp -std=c++20 -fopenmp -DL_MACRO=${sizes[i]} -DNUM_THREADS=${cores[i]} -O3 -o "./compiled/tri-"${sizes[i]}
done

for i in {0..3};
do
	sbatch --cpus-per-task=${cores[i]} --partition=${partition[i]} --time="${time[i]}:00:00" --mem=${mem[i]} --job-name="${sizes[i]}-tri" --array="0-99" job_scripts/run.sh ${sizes[i]} $1
done
