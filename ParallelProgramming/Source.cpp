#include <iostream>
#include <vector>
#include <fstream>

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


int main() {
	vector<vector<int>> a = {
		{1, 2, 3},
		{4, 5, 6}
	};
	vector<vector<int>> b = {
		{7, 8},
		{9, 10},
		{11, 12}
	};
	auto c = multiplyMatrices<int>(a, b);
	for (const auto& row : c) {
		for (auto value : row) {
			cout << value << " ";
		}
		cout << endl;
	}

	return 0;
}