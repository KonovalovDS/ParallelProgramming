from generator import generate_samples
from validator import is_correct
from stats import read_stats, plot_function


if __name__ == '__main__':
    #generate_samples()
    #print(is_correct())
    size_data, time_data = read_stats("ParallelProgramming\\stats.txt")
    plot_function(size_data, time_data)
 