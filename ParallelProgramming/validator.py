import numpy as np


def read_matrix_from_file(filename):
    with open(filename, 'r') as f:
        matrix = [list(map(int, line.strip().split(','))) for line in f]
    return np.array(matrix)

def multiply_matrices(size, index):
    file_a = f'ParallelProgramming\\samples\\samples_{size}_{index}_a'
    file_b = f'ParallelProgramming\\samples\\samples_{size}_{index}_b'
    matrix_a = read_matrix_from_file(file_a)
    matrix_b = read_matrix_from_file(file_b)
    result_matrix = np.dot(matrix_a, matrix_b)
    return result_matrix

def is_correct():
    flag = True
    for size in range(100, 1001, 100):
        print('Checking matrices with size: ', size)
        for index in range(10):
            file_c = f'ParallelProgramming\\samples\\samples_{size}_{index}_c'
            matrix_c = read_matrix_from_file(file_c)
            correct_c = multiply_matrices(size, index)
            with open(f'ParallelProgramming\\samples\\samples_{size}_{index}_c_correct', 'w') as f:
                for row in correct_c:
                    f.write(','.join(map(str, row)) + '\n')
            for i in range(size):
                for j in range(size):
                    if (matrix_c[i][j] != correct_c[i][j]):
                        return False
    return True
 