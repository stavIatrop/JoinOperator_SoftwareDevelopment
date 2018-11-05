#ifndef VICE
#define VICE
#include "basicStructs.h"

uint32_t Hash1_2(int32_t, uint32_t); 	//The hash function used. A simple %, only used with powers of 2.

uint32_t FindNextPower(uint32_t); 	//Given an uint32_t, returns the next power of 2.

double IdenticalityTest(relation *); 	//Calculates, using statistics, the approximate percentage of equal keys in the relation. Works
					//better for larger relations. Only works accurately if there is at most 1 repeating value.

void rSwap(tuple *,uint32_t *, uint32_t, uint32_t); //Swaps two elements in a tupleArray, and the same two elements in an intArray

uint32_t DoTheHash(relation *, uint32_t, uint32_t *, uint32_t *, uint32_t *, char trivial); //Sets the values of hash_values and hist for a
											    //given relation and number of buckets.

uint32_t *Hash1(relation *,uint32_t *, uint32_t *); //calculates the best number of buckets for a given relation and available cache size,
						    //performs the hashing and sets all the appropriate structs.

#endif
