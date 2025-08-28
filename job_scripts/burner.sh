module purge all
module load gcc/12.3.0-gcc

declare -a sizes=(8 12 16 24 32 48 64 96 128)
declare -a cores=(1 1 1 1 1 4 4 8 8)
declare -a time=(1 1 1 1 2 4 12 150 250)
declare -a mem=(20 20 20 20 30 30 30 40 40)

for i in {7..8};
do
	g++ ./main.cpp -std=c++20 -fopenmp -DL_MACRO=${sizes[i]} -DNUM_THREADS=${cores[i]} -O3 -o "./compiled/tri-"${sizes[i]}
done

for i in {7..8};
do
	sbatch --cpus-per-task=${cores[i]} --time="${time[i]}:00:00" --mem=${mem[i]} --job-name="${sizes[i]}-triburn" job_scripts/burn.sh ${sizes[i]} $1
done
