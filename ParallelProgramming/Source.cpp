#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>

using namespace std;


template<typename T>
vector<vector<T>> multiplyMatrices(const vector<vector<T>>& left, const vector<vector<T>>& right) {
	vector<vector<T>> result;
	if (left[0].size() == right.size() && !left.empty() && !right.empty()) {
		result.resize(left.size(), vector<T>(right[0].size(), 0));
		for (auto i = 0; i < left.size(); ++i) {
			for (auto j = 0; j < right[0].size(); ++j) {
				for (auto k = 0; k < left[0].size(); ++k) {
					result[i][j] += left[i][k] * right[k][j];
				}
			}
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
            c = multiplyMatrices<T>(a, b);
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