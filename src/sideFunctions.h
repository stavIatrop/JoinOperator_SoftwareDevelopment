#ifndef SIDE_FUNCTIONS_H
#define SIDE_FUNCTIONS_H

#include "basicStructs.h"

relation * getStartOfBucket(reorderedR * , int );
void buildIndex(relationIndex * , reorderedR *, int , int , int );
void updateChain(uint32_t * , uint32_t * , int , int );

#endif