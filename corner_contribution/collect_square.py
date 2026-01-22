import os
import csv
import re
import sys
from glob import glob

# ---- CONFIG ----
INPUT_DIR = sys.argv[1]
OUTPUT_DIR = sys.argv[1]
MAX_INDEX = 20

os.makedirs(OUTPUT_DIR, exist_ok=True)

# Prepare writers for 0.csv ... 20.csv
writers = {}
files = {}
for i in range(MAX_INDEX + 1):
    f = open(os.path.join(OUTPUT_DIR, f"{i}.csv"), "w", newline="")
    writer = csv.writer(f)
    writer.writerow(["idx", "gamma", "corner_contribution", "L", "batch", "standard_error"])
    writers[i] = writer
    files[i] = f

# Regex for filename: bp_{run}_{l}_0_123
pattern = re.compile(r"bp_(\d+)_(\d+)_(\d+)_(\d+)\.cor$")

for filepath in glob(os.path.join(INPUT_DIR, "*.cor")):
    filename = os.path.basename(filepath)
    match = pattern.match(filename)
    if not match:
        continue

    run = int(match.group(1))
    l = int(match.group(2))

    with open(filepath, "r") as f:
        for line in f:
            parts = line.strip().split("\t")
            if len(parts) < 6:
                continue  # skip malformed lines

            idx = int(parts[0])
            if 0 <= idx <= MAX_INDEX:
                # row: [idx, five values, l, run]
                row = [parts[0], parts[1], parts[3]] + [l, run, 0]
                writers[idx].writerow(row)


# Close all output files
for f in files.values():
    f.close()

print("Finished generating CSVs.")
