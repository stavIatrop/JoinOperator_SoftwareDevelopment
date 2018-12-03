#ifndef CHECKSUM_INTERFACE_H
#define CHECKSUM_INTERFACE_H

#include <stdint.h>

#include "queryStructs.h"

typedef struct Checksum {
    myint_t * checksums;
    myint_t numbOfChecksums;
} checksum;

checksum * performChecksums(colRel * sums, int numbOfSums, headInter * headInt);
myint_t calcChecksum(myint_t ** intValues, myint_t intRows, myint_t intCol,  myint_t * relValues);

#endif
