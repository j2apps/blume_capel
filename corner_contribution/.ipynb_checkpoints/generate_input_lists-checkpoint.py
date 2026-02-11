import os
import sys
from pathlib import Path

if __name__ == "__main__":
    root = sys.argv[1]
    Path(f'./{root}/gap/square').mkdir(parents=True, exist_ok=True)
    Path(f'./{root}/gap/square/fk').mkdir(parents=True, exist_ok=True)
    Path(f'./{root}/gap/square/spin').mkdir(parents=True, exist_ok=True)
    Path(f'./{root}/gap/square/inputs').mkdir(parents=True, exist_ok=True)
    for entropy in ('fk', 'spin'):
        Path(f'./{root}/gap/square/inputs/{entropy}').mkdir(parents=True, exist_ok=True)
        for l in (16, 24, 32, 48, 64):
            Path(f'./{root}/gap/square/inputs/{entropy}/{l}').mkdir(parents=True, exist_ok=True)
            for batch in range(100):
                input_list_fname = f'./{root}/gap/square/inputs/{entropy}/{l}/{batch}.txt'
                with open(input_list_fname, 'w') as file:
                    run = batch
                    for root2, _, files in os.walk(f'./{root}/{entropy}/{l}/{run}/lattice'):
                        for sample_fname in files:
                            sample_full_path = os.path.join(root2, sample_fname)
                            file.write(f'{sample_full_path}\n')
