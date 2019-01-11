#ifndef HASHTREE_H
#define HASHTREE_H

#include "basicStructs.h"
#include "queryStructs.h"

typedef struct hashTreeStats {

    stats * relStats;       //array of structs for each column of relation
    myint_t cols;
    myint_t rel;

} HTStats;

typedef struct hashTreeNode {

    HTStats * htstats;      //array to keep statistics of each rel that exists in set
    myint_t numRels;
    char * comb;
    myint_t cost;

}HTNode;

HTNode * InitialiseHTNode(query *, myint_t , myint_t  );
HTNode ** InitialiseHashTree(myint_t , query *, relationsheepArray);
myint_t countSetBits(myint_t );
myint_t TestBitPos(myint_t , myint_t );
void FreeHashTree(HTNode ** , myint_t );
myint_t charToInteger(char );
myint_t combToIndex(char *);

#endif