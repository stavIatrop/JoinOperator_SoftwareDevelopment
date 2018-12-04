#ifndef INDEX_MANIP_H
#define INDEX_MANIP_H

#include "basicStructs.h"

relationIndex initializeIndex(myint_t , relation * , myint_t , relationIndex * , myint_t );
indexArray * initializeIndexArray(myint_t );
void freeIndexArray(indexArray * idxArray);

#endif