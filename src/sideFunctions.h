#ifndef SIDE_FUNCTIONS_H
#define SIDE_FUNCTIONS_H

#include <stdint.h>

#include "basicStructs.h"

relation * getStartOfBucket(reorderedR * , myint_t );
void buildIndex(relationIndex * , myint_t, myint_t );
void updateChain(myint_t * , myint_t * , myint_t , myint_t );
void buildSubIndex(relationIndex ** , myint_t , myint_t, myint_t , myint_t , myint_t , tuple * , myint_t );
relation * getStartOfSubBucket(tuple *, myint_t , myint_t );
_Bool IsPowerOfTwo(myint_t );
myint_t findPowerOf2Hash(myint_t );
void recomputeSizes(myint_t * , myint_t * , myint_t ); 


#endif