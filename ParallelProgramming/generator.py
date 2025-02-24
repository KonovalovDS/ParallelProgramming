import numpy as np

def generate_samples(size):
    for i in range(10):
        matrix_a = np.random.randint(0, 10000, size=(size, size)).tolist()
        matrix_b = np.random.randint(0, 10000, size=(size, size)).tolist()
        with open(f'ParallelProgramming\\samples\\samples_{size}_{i}_a', 'w') as f:
            for row in matrix_a:
                f.write(','.join(map(str, row)) + '\n')
        with open(f'ParallelProgramming\\samples\\samples_{size}_{i}_b', 'w') as f:
            for row in matrix_b:
                f.write(','.join(map(str, row)) + '\n')
