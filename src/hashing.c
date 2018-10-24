#include <stdint.h>

int hash1Range(int hash1) {

	return hash1;
}

int hash2Range(int hash2) {

	return hash2;
}

uint32_t hashing(int value, int hash) {

	return value % hash;
}