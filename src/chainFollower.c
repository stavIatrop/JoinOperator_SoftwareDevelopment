#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "basicStructs.h"
#include "resultListInterface.h"
#include "hashing.h"
#include "chainFollowerInterface.h"

//DO NOT KNOW IF CACHE LOCALLITY IS ACHIEVED DUE TO POINTERS
void followChain(headResult * resultList, relationIndex rIndex, tuple t, int h2) {
        uint32_t hashRes = performHash(t.payload, h2);
        uint32_t chainPointer = rIndex.buckets[hashRes] - 1;
        //Case: No matching h2
        if(chainPointer == EMPTY_BUCKET) {
                return;
        }
        while(1) {
                if(rIndex.rel->tuples[chainPointer].payload == t.payload) {
                        //printf("RowId = ")
                        tuple * temp = (tuple *) malloc(sizeof(tuple));
                        temp->key = rIndex.rel->tuples[chainPointer].key;
                        temp->payload = t.key;
                        pushResult(resultList, temp);
                        free(temp);
                }

                if(rIndex.chain[chainPointer] != 0) {
                        chainPointer = rIndex.chain[chainPointer] - 1;
                }
                else {
                        return;
                }
        }
}

void searchKey(indexArray indArr, headResult * resultList, tuple * checkedTuples, int tuplesNumb, int key, int h2) {
        relationIndex keyIndex = indArr.indexes[key];

        for(int whichTup = 0; whichTup < tuplesNumb; whichTup++) {
                followChain(resultList, keyIndex, checkedTuples[whichTup], h2);
        }
}

headResult * search(indexArray indArr, reorderedR * s, int hash2) {
        int size = -1;
        tuple * startTup = NULL;
        int key1 = -1;
        headResult * resultList = initialiseResultHead();
        for(int whichKey = 0; whichKey < s->psumSize; whichKey++) {
                if(whichKey < s->psumSize - 1) {
                        size = s->psum[whichKey + 1].offset -  s->psum[whichKey].offset;
                }
                else {
                        size = s->rel.size;
                }
                key1 = s->psum[whichKey].h1Res;
                startTup = &(s->rel.tuples[s->psum[whichKey].offset]);

                searchKey(indArr, resultList, startTup, size, key1, hash2);
        }

        return resultList;
}
