import numpy as np
import matplotlib.pyplot as plt


def read_stats(filename):
    x_values = []
    y_values = []
    try:
        with open(filename, 'r') as file:
            for line in file:
                line = line.strip()
                if line:
                    x, y = map(float, line.split(','))
                    x_values.append(x)
                    y_values.append(y)
    except FileNotFoundError:
        print(f"Ошибка: Файл '{filename}' не найден.")
    except ValueError:
        print("Ошибка: Неверный формат данных в файле.")
    
    return x_values, y_values

def plot_function(x, y):
    plt.figure(figsize=(10, 5))
    plt.plot(x, y, label="Длительность умножения")
    plt.title("Зависимость длительности умножения от размера исходных матриц")
    plt.xlabel("Размер матриц, ед.")
    plt.ylabel("Время, сек")
    plt.axhline(0, color='black', linewidth=0.5, ls='--')
    plt.axvline(0, color='black', linewidth=0.5, ls='--')
    plt.grid()
    plt.legend()
    plt.savefig("ParallelProgramming\stats.png")
    plt.show()
 