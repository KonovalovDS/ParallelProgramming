import numpy as np

def read_matrix_from_file(filename):
    with open(filename, 'r') as f:
        matrix = [list(map(int, line.strip().split(','))) for line in f]
    return np.array(matrix)

def multiply_matrices(size, index):
    file_a = f'ParallelProgramming\samples\samples_{size}_{index}_a'
    file_b = f'ParallelProgramming\samples\samples_{size}_{index}_b'
    matrix_a = read_matrix_from_file(file_a)
    matrix_b = read_matrix_from_file(file_b)
    result_matrix = np.dot(matrix_a, matrix_b)
    return result_matrix