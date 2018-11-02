#ifndef VICE
#define VICE
#include "basicStructs.h"

uint32_t Hash1_2(int32_t, uint32_t);

uint32_t FindNextPower(uint32_t);

double IdenticalityTest(relation *);

void rSwap(tuple *,uint32_t *, uint32_t, uint32_t);

uint32_t DoTheHash(relation *, uint32_t, uint32_t *, uint32_t *, uint32_t *);

uint32_t *Hash1(relation *,uint32_t *, uint32_t *);

#endif
