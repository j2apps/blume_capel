module purge all
module load gcc/12.3.0-gcc

g++ ./corner_contribution/gamma_nu.cpp -std=c++20 -fopenmp -DNUM_THREADS=4 -o ./compiled/gn

python3 ./corner_contribution/gen_gss_files.py ./corner_contribution/data/gn_tricritical-7-29

./compiled/gn ./data/tricritical-7-29/spin ./corner_contribution/data/gn_tricritical-7-29/spin_gn.txt
./compiled/gn ./data/tricritical-7-29/fk ./corner_contribution/data/gn_tricritical-7-29/fk_gn.txt




