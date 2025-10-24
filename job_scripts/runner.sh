module purge all
module load gcc/12.3.0-gcc

declare -a sizes=(8 12 16 24 32 48 64)
declare -a nsamples=(15000 15000 15000 15000 5000 5000 1500)
declare -a nruns=(100 100 50 50 50 50 50)
declare -a cores=(1 1 1 1 2 4 4)
declare -a partition=(short short normal normal normal long long)	
declare -a time=(1 1 48 48 48 168 168)
declare -a mem=(20 20 30 30 40 40 50)

for i in {0..6};
do
	g++ ./main.cpp -std=c++20 -fopenmp -DL_MACRO=${sizes[i]} -DNUM_THREADS=${cores[i]} -O3 -o "./compiled/${5}-${sizes[i]}"
done

for i in {0..6};
do
	sbatch --cpus-per-task=${cores[i]} --partition=${partition[i]} --time="${time[i]}:00:00" --mem=${mem[i]} --job-name="${5}-${sizes[i]}" --array="0-$((nruns[i]-1))" job_scripts/run.sh ${sizes[i]} $1 ${nsamples[i]} $2 $3 $4 $5
done
#1:file, 2:T, 3:D, 4:J, 5:name
