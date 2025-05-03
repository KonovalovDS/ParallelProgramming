#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include "cuda_runtime.h"
#include <device_launch_parameters.h>

using namespace std;

template<typename T>
__global__ void matrixMultiplyKernel(T* left, T* right, T* result, int left_rows, int left_cols, int right_cols) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < left_rows && col < right_cols) {
        int sum = 0;
        for (int k = 0; k < left_cols; k++) {
            sum += left[row * left_cols + k] * right[k * right_cols + col];
        }
        result[row * right_cols + col] = sum;
    }
}

template<typename T>
vector<vector<T>> multiplyMatricesCUDA(const vector<vector<T>>& left, const vector<vector<T>>& right) {
    int left_rows = left.size();
    int left_cols = left[0].size();
    int right_rows = right.size();
    int right_cols = right[0].size();
    if (left_cols != right_rows || left.empty() || right.empty()) {
        cout << "Multiply error: cannot multiply matrices with this sizes: " << left_cols << "x" << right_rows << endl;
        return {};
    }

    vector<T> flat_left(left_rows * left_cols);
    vector<T> flat_right(right_rows * right_cols);
    vector<T> flat_result(left_rows * right_cols);

    for (int i = 0; i < left_rows; i++) {
        for (int j = 0; j < left_cols; j++) {
            flat_left[i * left_cols + j] = left[i][j];
        }
    }
    for (int i = 0; i < right_rows; i++) {
        for (int j = 0; j < right_cols; j++) {
            flat_right[i * right_cols + j] = right[i][j];
        }
    }

    T* d_left, * d_right, * d_result;
    cudaMalloc(&d_left, left_rows * left_cols * sizeof(T));
    cudaMalloc(&d_right, right_rows * right_cols * sizeof(T));
    cudaMalloc(&d_result, left_rows * right_cols * sizeof(T));

    cudaMemcpy(d_left, flat_left.data(), left_rows * left_cols * sizeof(T), cudaMemcpyHostToDevice);
    cudaMemcpy(d_right, flat_right.data(), right_rows * right_cols * sizeof(T), cudaMemcpyHostToDevice);

    dim3 threadsPerBlock(16, 16);
    dim3 numBlocks((right_cols + threadsPerBlock.x - 1) / threadsPerBlock.x, (left_rows + threadsPerBlock.y - 1) / threadsPerBlock.y);

    matrixMultiplyKernel<<<numBlocks, threadsPerBlock>>>(d_left, d_right, d_result, left_rows, left_cols, right_cols);

    cudaMemcpy(flat_result.data(), d_result, left_rows * right_cols * sizeof(T), cudaMemcpyDeviceToHost);

    cudaFree(d_left);
    cudaFree(d_right);
    cudaFree(d_result);

    vector<vector<T>> result(left_rows, vector<T>(right_cols));
    for (int i = 0; i < left_rows; i++) {
        for (int j = 0; j < right_cols; j++) {
            result[i][j] = flat_result[i * right_cols + j];
        }
    }

    return result;
}

template<typename T>
vector<vector<T>> readMatrix(const string& filepath) {
    vector<vector<T>> matrix;
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "File opening error: " << filepath << endl;
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
    for (int i = 100; i <= 1000; i += 100) {
        duration = chrono::duration<double>(0);
        for (int j = 0; j < 10; ++j) {
            cout << i << " " << j << endl;
            filepath = path + "_" + to_string(i) + "_" + to_string(j) + "_";
            a = readMatrix<T>(filepath + "a");
            b = readMatrix<T>(filepath + "b");
            start = chrono::high_resolution_clock::now();
            c = multiplyMatricesCUDA<T>(a, b);
            end = chrono::high_resolution_clock::now();
            writeMatrix(c, filepath + "c");
            duration += end - start;
        }
        stats.push_back(pair<int, double>(i, duration.count() / 10.0));
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
    auto stats = testMultiplication<int>("samples\\samples");
    writeStats(stats, "stats.txt");
    return 0;
}