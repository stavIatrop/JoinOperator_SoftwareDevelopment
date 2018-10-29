#ifndef SIDE_FUNCTIONS_H
#define SIDE_FUNCTIONS_H

#include <stdint.h>

#include "basicStructs.h"

relation * getStartOfBucket(reorderedR * , uint32_t );
void buildIndex(relationIndex * , uint32_t );
void updateChain(uint32_t * , uint32_t * , uint32_t , uint32_t );

#endif