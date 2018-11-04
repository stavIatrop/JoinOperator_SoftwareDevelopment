#include <stdint.h>
#include <stdio.h>
#include <math.h>

uint32_t hash1Range(uint32_t hash1) {

	return pow(2,hash1);
}

uint32_t hash2Range(uint32_t hash2) {

	return pow(2, hash2);
}

uint32_t hashing(int32_t value, uint32_t hash1, uint32_t hash2) {


	value >>= hash1;
	
	uint32_t range = hash2Range(hash2);
	return ( value ^ (value >> (hash1 + 3)) ^ (value >> (hash1 + 5) )) & (range - 1);	
}
