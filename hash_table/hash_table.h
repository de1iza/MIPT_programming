#include <vector>

#include "list.h"

template <typename T>
class HashTable {
	using list_t = List<T>;
private:
	size_t size;

	list_t* data;
	uint32_t(*hash_func) (const T&);

	size_t GetIndex(const T& val) const {
		return hash_func(val) % size;
	}

public:
	HashTable(): size(0), hash_func(nullptr) {}

	HashTable(size_t size, uint32_t(*func) (const T&)): size(size), hash_func(func) {
		data = new list_t[size];
		for (size_t i = 0; i < size; ++i) {
			data[i] = list_t();
		}
	}

	~HashTable() {
		delete[] data;
	}

	void Insert(const T& val) {
		size_t idx = GetIndex(val);
		data[idx].Insert(val);
	}

	bool Remove(const T& val) {
		size_t idx = GetIndex(val);
		return data[idx].Remove(val);
	}

	void Show() const {
		for (size_t i = 0; i < size; ++i) {
			std::cout << i << ": ";
			data[i].Show();
		}
	}

	void DumpLens(std::ofstream& out) const {
		for (auto& row : data) {
			out << row.GetSize() << "\n";
		}
	}

	bool Contains(const T& val) const {
		size_t idx = GetIndex(val);
		return data[idx].Contains(val);
	}


};