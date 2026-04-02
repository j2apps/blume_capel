dirs=("t2_d3.5" "t2_d4" "t3_d2" "t3_d4" "t3.5_d2.5" "t3.5_d4" "t4_d3" "t4_d3.5" "t4_d4")
root="/projects/p32813/blume_capel/data"
for file in $root/sweep_archives/*; do
    sbatch job_scripts/compress.sh $file
done

# total: 27
# n=5
# for dir in "${dirs[@]}"; do

# sbatch --job-name="tar_$dir" job_scripts/archive.sh "$fullpath" "/projects/p32813/blume_capel/data/sweep_archives/$dir.tar" 
# mv "$root/sweep_to_archive/$dir" "$root/trash"
# sbatch job_scripts/delete_file.sh "$root/trash/$dir"
# done

# 2,2.5 2,2 2,3.5 2,3 2.5,3.5 2.5,4