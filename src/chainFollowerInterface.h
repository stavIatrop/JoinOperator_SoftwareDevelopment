#ifndef CHAIN_FOLLOWER_H
#define CHAIN_FOLLOWER_H

#include "basicStructs.h"

#define EMPTY_BUCKET -1

void followChain(headResult * resultList, relationIndex rIndex, tuple t, int h2);
void searchKey(indexArray indArr, headResult * resultList, tuple * checkedTuples, int tuplesNumb, int key, int h2);

#endif