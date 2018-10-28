#include <stdint.h>

int hash1Range(uint32_t hash1) {

	return hash1;
}

int hash2Range(uint32_t hash2) {

	return hash2;
}

uint32_t hashing(uint32_t value, uint32_t hash) {

	return value % hash;
}