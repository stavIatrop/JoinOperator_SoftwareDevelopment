#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "basicStructs.h"

myint_t hash1Range(myint_t hash1) {

	return pow(2,hash1);
}

myint_t hash2Range(myint_t hash2) {

	return pow(2, hash2);
}

myint_t hashing(myint_t value, myint_t hash1, myint_t hash2) {


	value >>= hash1;
	
	myint_t range = hash2Range(hash2);
	return ( value ^ (value >> (hash1 + 3)) ^ (value >> (hash1 + 5) )) & (range - 1);	
}
