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
vector<vector<T>> readMatrix(const string& path, const int& size, const int& index, const string& name) {
    vector<vector<T>> matrix;
    string filepath = path + "_" + to_string(size) + "_" + to_string(index) + "_" + name;
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

vector<pair<int, double>> testMultiplication(const string& path) {
    chrono::steady_clock::time_point start;
    chrono::steady_clock::time_point end;
    chrono::duration<double> duration;
    vector<vector<int>> a;
    vector<vector<int>> b;
    vector<vector<int>> c;
    vector<pair<int, double>> stats;
    for (int i = 100; i <= 300; i += 100) {
        duration = chrono::duration<double>(0);
        for (int j = 0; j < 10; ++j) {
            cout << i << " " << j << endl;
            a = readMatrix<int>(path, i, j, "a");
            b = readMatrix<int>(path, i, j, "b");
            start = chrono::high_resolution_clock::now();
            c = multiplyMatrices(a, b);
            end = chrono::high_resolution_clock::now();
            duration += end - start;
        }
        stats.push_back(pair<int, double>(i, duration.count()));
    }
    return stats;
}


int main(int argc, char* argv[]) {
    auto stats = testMultiplication("samples\\samples");
    for (const auto& test : stats) {
        cout << test.first << " : " << test.second << endl;
    }
    return 0;
}