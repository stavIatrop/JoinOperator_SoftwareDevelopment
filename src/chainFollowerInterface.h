#ifndef CHAIN_FOLLOWER_H
#define CHAIN_FOLLOWER_H

#include "basicStructs.h"

#define EMPTY_BUCKET -1

void followChain(headResult * resultList, relationIndex * rIndex, tuple t);
void searchKey(indexArray * indArr, headResult * resultList, tuple * checkedTuples, uint32_t tuplesNumb, uint32_t key);
void searchKeyRec(relationIndex * keyIndex, headResult * resultList, tuple * checkedTuples, uint32_t tuplesNumb);

#endif