from generator import generate_samples
from validator import multiply_matrices, read_matrix_from_file

#result = multiply_matrices(10, 0)
#print(f'Result of multiplication for size {10}, sample {0}:\n{result}\n')

for i in range(100, 1001, 100):
    generate_samples(i)