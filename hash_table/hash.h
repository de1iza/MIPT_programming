#include <string>

uint32_t Hash1(const std::string& str) {
	return 1;
}

uint32_t LengthHash(const std::string& str) {
	return str.length();
}

uint32_t AsciiSumHash(const std::string& str) {
	uint32_t hash = 0;
	size_t len = str.length();

	for (size_t i = 0; i < len; ++i) {
		hash += str[i];
	}
	return hash;
}

uint32_t AvgAsciiHash(const std::string& str) {
	uint32_t hash = 0;
	size_t len = str.length();

	for (size_t i = 0; i < len; ++i) {
		hash += str[i];
	}
	return len ? hash / len : 0;
}

uint32_t XORHash(const std::string& str) {
	uint32_t hash = 0;
	size_t len = str.length();

	for (size_t i = 0; i < len; ++i) {
		hash ^= str[i];
		hash = (hash << 1) | (hash >> 31);
	}
	return hash;
}

uint32_t MurMurHash(const std::string& str) {
	const char* key = str.c_str();
	int len = str.length();
	const unsigned int m = 0x5bd1e995;
	const unsigned int seed = 0;
	const int r = 24;

	unsigned int h = seed ^ len;

	const unsigned char* data = (const unsigned char*)key;
	unsigned int k;

	while (len >= 4)
	{
		k = data[0];
		k |= data[1] << 8;
		k |= data[2] << 16;
		k |= data[3] << 24;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	switch (len)
	{
	case 3:
		h ^= data[2] << 16;
	case 2:
		h ^= data[1] << 8;
	case 1:
		h ^= data[0];
		h *= m;
	};

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

extern "C" {
	uint32_t _XORHashAsm(const char* str, int len);
}


uint32_t XORHashAsm(const std::string& str) {
	return _XORHashAsm(str.c_str(), str.length());
}