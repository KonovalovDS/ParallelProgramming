from generator import generate_samples
from validator import is_correct, delete_matrices
from stats import read_stats, plot_function


if __name__ == '__main__':
    #generate_samples()
    #delete_matrices()
    if is_correct():
        size_data, time_data = read_stats("ParallelProgramming\\stats.txt")
        plot_function(size_data, time_data) 
 