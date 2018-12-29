#ifndef VICE
#define VICE
#include "basicStructs.h"

myint_t Hash1_2(myint_t, myint_t); 	//The hash function used. A simple %, only used with powers of 2.

myint_t FindNextPower(myint_t); 	//Given an myint_t, returns the next power of 2.

double IdenticalityTest(relation *); 	//Calculates, using statistics, the approximate percentage of equal keys in the relation. Works
					//better for larger relations. Only works accurately if there is at most 1 repeating value.

void rSwap(tuple *,myint_t *, myint_t, myint_t); //Swaps two elements in a tupleArray, and the same two elements in an intArray

myint_t DoTheHash(relation *, myint_t, myint_t *, myint_t *, myint_t *, char trivial); //Sets the values of hash_values and hist for a
											    //given relation and number of buckets.

myint_t *Hash1(relation *,myint_t, myint_t *); //calculates the best number of buckets for a given relation and available cache size,
						    //performs the hashing and sets all the appropriate structs.

#endif
