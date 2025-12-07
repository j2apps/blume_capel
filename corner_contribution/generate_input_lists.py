import os
import sys
from pathlib import Path

if __name__ == "__main__":
    root = sys.argv[1]
    Path(f'./{root}/inputs').mkdir(parents=True, exist_ok=True)
    for entropy in ('fk', 'spin'):
        Path(f'./{root}/inputs/{entropy}').mkdir(parents=True, exist_ok=True)
        for l in (8, 12, 16, 24, 32, 48, 64):
            Path(f'./{root}/inputs/{entropy}/{l}').mkdir(parents=True, exist_ok=True)
            for batch in range(10):
                filename=f'./{root}/inputs/{entropy}/{l}/{batch}.txt'
                with open(filename, 'w') as file:
                    for i in range(10):
                        run = batch*10 + i
                        for root, _, files in os.walk(f'./{root}/{entropy}/{l}/{run}'):
                            for file_name in files:
                                full_path = os.path.join(root, file_name)
                                file.write(f'{full_path}\n')
