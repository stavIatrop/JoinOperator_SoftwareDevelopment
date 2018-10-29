#include <stdint.h>

uint32_t hash1Range(uint32_t hash1) {

	return hash1;
}

uint32_t hash2Range(uint32_t hash2) {

	return hash2;
}

uint32_t hashing(int32_t value, uint32_t hash) {

	return value % hash;
}
