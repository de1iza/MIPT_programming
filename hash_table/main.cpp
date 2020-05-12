#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <chrono>

#include "hash.h"
#include "hash_table.h"


template <typename T>
double TimeTest(HashTable<T>& table, std::vector<T>& values, int n_iterations);
bool ReadData(const std::string& filename, std::vector<std::string>& lines);

/*
int main() {
	const int TABLE_SIZE = 3671;

	std::vector<std::string> data;
	HashTable<std::string> table = HashTable<std::string>(TABLE_SIZE, XORHash);

	if (!ReadData("data.txt", data))
		return -1;

	std::cout << TimeTest<std::string>(table, data, 100);

	return 0;
}*/

template <typename T>
double TimeTest(HashTable<T>& table, const std::vector<T>& values, const int n_iterations) {

	std::clock_t c_start = std::clock();
	for (int i = 0; i < n_iterations; ++i) {
		for (int j = 0; j < values.size(); ++j) {
			table.Insert(values[j]);
			table.Contains(values[j]);
		}

	}
	std::clock_t c_end = std::clock();
	double time_elapsed_ms = 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC;

	return time_elapsed_ms;
}


bool ReadData(const std::string& filename, std::vector<std::string>& lines) {
	std::ifstream input(filename);
	if (!input) {
		std::cerr << "Can't open file\n";
		return false;
	}

	std::string s = "";
	while (input >> s) {
		lines.push_back(s);
	}
	input.close();

	return true;
}


