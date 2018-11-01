#ifndef SIDE_FUNCTIONS_H
#define SIDE_FUNCTIONS_H

#include <stdint.h>

#include "basicStructs.h"

relation * getStartOfBucket(reorderedR * , uint32_t );
void buildIndex(relationIndex * , uint32_t, uint32_t );
void updateChain(uint32_t * , uint32_t * , uint32_t , uint32_t );
void buildSubIndex(relationIndex ** , uint32_t , uint32_t, uint32_t , uint32_t , uint32_t , tuple * , uint32_t );
relation * getStartOfSubBucket(tuple *, uint32_t , uint32_t );
_Bool IsPowerOfTwo(uint32_t );
uint32_t findPowerOf2Hash(uint32_t );


#endif