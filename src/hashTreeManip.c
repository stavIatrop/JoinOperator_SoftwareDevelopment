#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hashTreeManip.h"
#include "pipeI_O.h"

HTNode ** InitialiseHashTree(myint_t size, query * newQuery, relationsheepArray relArray){

    HTNode ** newHTree = (HTNode **) malloc(size * sizeof(HTNode *));
    for(int i = 0; i < size; i++)  {

        newHTree[i] = NULL;
    }
    
    myint_t index;
    for(int i = 0; i < newQuery->numOfRels; i++) {

        index = 1 << i;
        myint_t numCols = relArray.rels[newQuery->rels[i]].cols;
        newHTree[index] = InitialiseHTNode(newQuery, i, numCols);
    }

    return newHTree;    
}


HTNode * InitialiseHTNode(query *newQuery, myint_t rel, myint_t numCols) {

    HTNode * newHTNode = (HTNode *) malloc(sizeof(HTNode));
    newHTNode->comb = (char *)malloc((numDigits(rel) + sizeof(" ") + sizeof("\0")) * sizeof(char));
    memset(newHTNode->comb, '\0', numDigits(rel) + sizeof(" ") + sizeof("\0") );
    sprintf(newHTNode->comb, "%d", rel);
    strcat(newHTNode->comb, " ");
    strcat(newHTNode->comb, "\0");

    newHTNode->numRels = 1;
    newHTNode->htstats = (HTStats *) malloc(newHTNode->numRels * sizeof(HTStats));
    newHTNode->htstats[0].cols = numCols;
    newHTNode->htstats[0].rel = rel;
    newHTNode->htstats[0].relStats = (stats *) malloc(numCols * sizeof(stats));

    for(int c = 0; c < numCols; c++) {

        newHTNode->htstats[0].relStats[c].minI = newQuery->queryStats[rel][c].minI;
        newHTNode->htstats[0].relStats[c].maxU = newQuery->queryStats[rel][c].maxU;
        newHTNode->htstats[0].relStats[c].distinctVals = newQuery->queryStats[rel][c].distinctVals;
        newHTNode->htstats[0].relStats[c].numElements = newQuery->queryStats[rel][c].numElements;
    }
    newHTNode->cost = newQuery->queryStats[rel][0].numElements;
    return newHTNode;

}


void FreeHashTree(HTNode ** hashTree, myint_t queryRels) {

    for(int i = 0; i < pow(2, queryRels); i++) {

        if(hashTree[i] != NULL) {
            for(int j = 0; j < hashTree[i]->numRels; j++) {

                free(hashTree[i]->htstats[j].relStats);
            }
            free(hashTree[i]->htstats);
            free(hashTree[i]->comb);
            
            free(hashTree[i]);
        }
        
        
    }
    free(hashTree);
    return;
    
}

myint_t charToInteger(char c) {

    return c - '0';

}

myint_t hashFunction(char *comb) {

    //calc binary index of combination comb
    myint_t index;

    return index;

}
myint_t countSetBits(myint_t  n) {

  myint_t  count = 0; 
  while (n) {

    count += n & 1; 
    n >>= 1; 
  } 
  return count; 
} 



myint_t TestBit(myint_t n, myint_t pos) {

    myint_t flag = 1; 

    flag = flag << pos;     
    
    if ( n & flag )      
        return 1;                       
    else
        return 0;                       
}