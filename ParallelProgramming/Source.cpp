#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <mpi.h>

using namespace std;


template<typename T>
vector<vector<T>> multiplyMatricesMPI(const vector<vector<T>>& left, const vector<vector<T>>& right, int rank, int size) {
    int rows = left.size();
    int cols = right[0].size();
    int inner_dim = right.size();
    vector<vector<T>> result(rows, vector<T>(cols, 0));

    int rows_per_process = rows / size;
    int extra_rows = rows % size;
    int start_row = rank * rows_per_process + min(rank, extra_rows);
    int end_row = start_row + rows_per_process + (rank < extra_rows ? 1 : 0);

    for (int i = start_row; i < end_row; ++i) {
        for (int j = 0; j < cols; ++j) {
            for (int k = 0; k < inner_dim; ++k) {
                result[i][j] += left[i][k] * right[k][j];
            }
        }
    }

    if (rank == 0) {
        for (int p = 1; p < size; ++p) {
            int p_start = p * rows_per_process + min(p, extra_rows);
            int p_end = p_start + rows_per_process + (p < extra_rows ? 1 : 0);

            for (int i = p_start; i < p_end; ++i) {
                MPI_Recv(result[i].data(), cols, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    }
    else {
        for (int i = start_row; i < end_row; ++i) {
            MPI_Send(result[i].data(), cols, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    return result;
}

template<typename T>
vector<vector<T>> readMatrix(const string& filepath) {
    vector<vector<T>> matrix;
    ifstream file(filepath);
    if (!file.is_open()) {
        cout << "File opening error: " << filepath << endl;
        return matrix;
    }
    string line;
    while (getline(file, line)) {
        vector<T> row;
        stringstream ss(line);
        string value;
        while (getline(ss, value, ',')) {
            row.push_back(stoi(value));
        }
        matrix.push_back(row);
    }
    file.close();
    return matrix;
}

void writeMatrix(const vector<vector<int>>& matrix, const string& path) {
    ofstream outFile(path);
    if (!outFile) {
        cerr << "File opening error: " << path << endl;
        return;
    }
    for (const auto& row : matrix) {
        for (size_t j = 0; j < row.size(); ++j) {
            outFile << row[j];
            if (j < row.size() - 1) {
                outFile << ",";
            }
        }
        outFile << endl;
    }
    outFile.close();
}

template<typename T>
vector<pair<int, double>> testMultiplication(const string& path) {
    chrono::steady_clock::time_point start;
    chrono::steady_clock::time_point end;
    chrono::duration<double> duration;
    vector<vector<T>> a;
    vector<vector<T>> b;
    vector<vector<T>> c;
    vector<pair<int, double>> stats;
    string filepath;

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (int i = 100; i <= 1000; i += 100) {
        duration = chrono::duration<double>(0);
        for (int j = 0; j < 10; ++j) {
            if (rank == 0) {
                cout << "Testing " << i << "x" << i << " matrices, iteration " << j << endl;
                filepath = path + "_" + to_string(i) + "_" + to_string(j) + "_";
                a = readMatrix<T>(filepath + "a");
                b = readMatrix<T>(filepath + "b");
            }

            int matrices_size[4] = { 0 };
            if (rank == 0) {
                matrices_size[0] = a.size();
                matrices_size[1] = a[0].size();
                matrices_size[2] = b.size();
                matrices_size[3] = b[0].size();
            }
            MPI_Bcast(matrices_size, 4, MPI_INT, 0, MPI_COMM_WORLD);

            vector<T> matrix_a_part(matrices_size[0] * matrices_size[1]);
            vector<T> matrix_b_part(matrices_size[2] * matrices_size[3]);

            if (rank == 0) {
                for (int row = 0; row < matrices_size[0]; ++row) {
                    copy(a[row].begin(), a[row].end(), matrix_a_part.begin() + row * matrices_size[1]);
                }
                for (int row = 0; row < matrices_size[2]; ++row) {
                    copy(b[row].begin(), b[row].end(), matrix_b_part.begin() + row * matrices_size[3]);
                }
            }

            MPI_Bcast(matrix_a_part.data(), matrix_a_part.size(), MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(matrix_b_part.data(), matrix_b_part.size(), MPI_INT, 0, MPI_COMM_WORLD);
            a.resize(matrices_size[0]);
            b.resize(matrices_size[2]);
            for (int row = 0; row < matrices_size[0]; ++row) {
                a[row].resize(matrices_size[1]);
                copy(matrix_a_part.begin() + row * matrices_size[1],
                    matrix_a_part.begin() + (row + 1) * matrices_size[1],
                    a[row].begin());
            }
            for (int row = 0; row < matrices_size[2]; ++row) {
                b[row].resize(matrices_size[3]);
                copy(matrix_b_part.begin() + row * matrices_size[3],
                    matrix_b_part.begin() + (row + 1) * matrices_size[3],
                    b[row].begin());
            }

            MPI_Barrier(MPI_COMM_WORLD);
            start = chrono::high_resolution_clock::now();
            c = multiplyMatricesMPI<T>(a, b, rank, size);
            MPI_Barrier(MPI_COMM_WORLD);
            end = chrono::high_resolution_clock::now();

            if (rank == 0) {
                writeMatrix(c, filepath + "c");
                duration += end - start;
            }
        }
        if (rank == 0) {
            stats.push_back(pair<int, double>(i, duration.count() / 10.0));
        }
    }

    return stats;
}

void writeStats(vector<pair<int, double>>& stats, const string& filepath) {
    std::ofstream outFile(filepath);
    if (!outFile) {
        std::cerr << "File opening error: " << filepath << std::endl;
        return;
    }
    for (const auto& p : stats) {
        outFile << p.first << "," << p.second << std::endl;
    }
    outFile.close();
}


int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        cout << "Using " << size << " CPUs" << endl;
    }

    auto stats = testMultiplication<int>("samples\\samples");

    if (rank == 0) {
        writeStats(stats, "stats.txt");
    }

    MPI_Finalize();
    return 0;
}