dirs=("t2.5_d3" "t3.5_d3" "t3_d2.5" "t3_d3.5")

for dir in "${dirs[@]}"; do
    fullpath="./data/sweep/$dir"
    # mv "$fullpath/spin/48" "$fullpath/spin/48x"
    # python3 gen_files.py "$fullpath"
    sbatch "job_scripts/move.sh" $dir
done