import numpy as np

def generate_samples():
    for size in range(100, 1001, 100):
        for index in range(10):
            matrix_a = np.random.randint(0, 100, size=(size, size)).tolist()
            matrix_b = np.random.randint(0, 100, size=(size, size)).tolist()
            with open(f'ParallelProgramming\\samples\\samples_{size}_{index}_a', 'w') as f:
                for row in matrix_a:
                    f.write(','.join(map(str, row)) + '\n')
            with open(f'ParallelProgramming\\samples\\samples_{size}_{index}_b', 'w') as f:
                for row in matrix_b:
                    f.write(','.join(map(str, row)) + '\n')
