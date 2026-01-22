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
            Path(f'./{root}/gap/squareinputs/{entropy}/{l}').mkdir(parents=True, exist_ok=True)
            for batch in range(10):
                filename=f'./{root}/gap/square/inputs/{entropy}/{l}/{batch}.txt'
                with open(filename, 'w') as file:
                    for i in range(10):
                        run = batch*10 + i
                        for root2, _, files in os.walk(f'./{root}/lattice/{entropy}/{l}/{run}'):
                            for file_name in files:
                                full_path = os.path.join(root2, file_name)
                                file.write(f'{full_path}\n')
