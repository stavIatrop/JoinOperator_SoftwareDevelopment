#ifndef CHAIN_FOLLOWER_H
#define CHAIN_FOLLOWER_H

#include <pthread.h>
#include <unistd.h>

#include "basicStructs.h"

#define EMPTY_BUCKET -1

extern int connectedLists;
extern pthread_cond_t cond_finished_search;
extern pthread_mutex_t finished_search_mutex;


void followChain(headResult * resultList, relationIndex * rIndex, tuple t, myint_t h1);
void searchKeyRec(relationIndex * keyIndex, headResult * resultList, tuple * checkedTuples, myint_t tuplesNumb, myint_t h1);

typedef struct SearchArgument {
    indexArray * indArr;
    myint_t key1;
    tuple * startTup;
    myint_t size;
    headResult ** resultLists;
    myint_t whichResult;
    myint_t sBuckets;
} searchArgument;

#endif