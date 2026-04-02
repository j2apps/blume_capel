import os
import sys
from pathlib import Path

if __name__ == "__main__":
    dataset_root = sys.argv[1]
    lattice_root = sys.argv[2]
    n_batches = int(sys.argv[3])
    n_runs = int(sys.argv[4])
    n_runs_per_batch = n_runs // n_batches
    Path(f'{dataset_root}/gap/square').mkdir(parents=True, exist_ok=True)
    Path(f'{dataset_root}/gap/square/fk').mkdir(parents=True, exist_ok=True)
    Path(f'{dataset_root}/gap/square/spin').mkdir(parents=True, exist_ok=True)
    Path(f'{dataset_root}/gap/square/inputs').mkdir(parents=True, exist_ok=True)
    for entropy in ('spin',):
        Path(f'{dataset_root}/gap/square/inputs/{entropy}').mkdir(parents=True, exist_ok=True)
        for l in (16, 24, 32, 48, 64, 96):
            Path(f'{dataset_root}/gap/square/inputs/{entropy}/{l}').mkdir(parents=True, exist_ok=True)
            for batch in range(n_batches):
                input_list_fname = f'{dataset_root}/gap/square/inputs/{entropy}/{l}/{batch}.txt'
                with open(input_list_fname, 'w') as file:
                    for run in range(batch * n_runs_per_batch, (batch+1) * n_runs_per_batch):
                        for run_root, _, files in os.walk(f'{lattice_root}/{entropy}/{l}/{run}'):
                            for sample_fname in files:
                                sample_full_path = os.path.join(run_root, sample_fname)
                                file.write(f'{sample_full_path}\n')
