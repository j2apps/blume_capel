import os
import sys
from pathlib import Path

if __name__ == "__main__":
	root = sys.argv[1]
	Path(f'./{root}').mkdir(parents=True, exist_ok=True)
	Path(f'./{root}/spin').mkdir(parents=True, exist_ok=True)
	Path(f'./{root}/fk').mkdir(parents=True, exist_ok=True)
	for l in (8, 16, 32, 64, 128):
		Path(f'./{root}/spin/{l}').mkdir(parents=True, exist_ok=True)
		Path(f'./{root}/fk/{l}').mkdir(parents=True, exist_ok=True)
		for run in range(100):
			Path(f'./{root}/spin/{l}/{run}').mkdir(parents=True, exist_ok=True)
			Path(f'./{root}/fk/{l}/{run}').mkdir(parents=True, exist_ok=True)
