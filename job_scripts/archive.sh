#!/bin/bash
#SBATCH --account=b1140
#SBATCH --output=outputs/%x.txt
#SBATCH --error=outputs/%x.err
#SBATCH --time=48:00:00
#SBATCH --cpus-per-task=1
#SBATCH --mem=30M
#SBATCH --partition=b1140
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jonahkim2028@u.northwestern.edu

set -euo pipefail

# Usage:
# sbatch tar_all.slurm /path/to/source /path/to/archive.tar
SRC_DIR="$1"
TAR_FILE="$2"

echo "Starting tar at $(date)"
echo "Source: $SRC_DIR"
echo "Archive: $TAR_FILE"
echo

# --- Create archive ---
tar -cf "$TAR_FILE" -C "$(dirname "$SRC_DIR")" "$(basename "$SRC_DIR")"

echo
echo "Finished tar at $(date)"

echo "Starting verification + delete at $(date)"

# --- Verify archive before deleting ---
if [[ -s "$TAR_FILE" ]] && tar -tf "$TAR_FILE" > /dev/null; then
    if [[ -n "$SRC_DIR" && "$SRC_DIR" != "/" && -d "$SRC_DIR" ]]; then
        echo "Archive OK. Deleting $SRC_DIR"
        rm -rf "$SRC_DIR"
    else
        echo "Refusing to delete suspicious SRC_DIR: '$SRC_DIR'"
        exit 1
    fi
else
    echo "Archive verification failed. NOT deleting source."
    exit 1
fi

echo "Finished rm at $(date)"
