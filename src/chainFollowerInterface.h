#ifndef CHAIN_FOLLOWER_H
#define CHAIN_FOLLOWER_H

#include "basicStructs.h"

#define EMPTY_BUCKET -1

void followChain(headResult * resultList, relationIndex * rIndex, tuple t, myint_t h1);
void searchKeyRec(relationIndex * keyIndex, headResult * resultList, tuple * checkedTuples, myint_t tuplesNumb, myint_t h1);

#endif