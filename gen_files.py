import os

os.mkdir('./clusters')
os.mkdir('./clusters/spin')
os.mkdir('./clusters/fk')
for l in (8, 16, 32, 64, 128):
    os.mkdir(f'./clusters/spin/{l}')
    os.mkdir(f'./clusters/fk/{l}')
    for run in range(100):
        os.mkdir(f'./clusters/spin/{l}/{run}')
        os.mkdir(f'./clusters/fk/{l}/{run}')
