#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "basicStructs.h"
#include "resultListInterface.h"
#include "hashing.h"
#include "chainFollowerInterface.h"

//DO NOT KNOW IF CACHE LOCALITY IS ACHIEVED DUE TO POINTERS
void followChain(headResult * resultList, relationIndex * rIndex, tuple t) {
        uint32_t hashRes = hashing(t.key, rIndex->hash2);
        uint32_t chainPointer = rIndex->buckets[hashRes] - 1;
        //Case: No matching h2
        if(chainPointer == EMPTY_BUCKET) {
                return;
        }
        while(1) {
                if(rIndex->rel->tuples[chainPointer].key == t.key) {
                        //printf("RowId = ")
                        rowTuple * temp = (rowTuple *) malloc(sizeof(rowTuple));
                        temp->rowR = rIndex->rel->tuples[chainPointer].payload;
                        temp->rowS = t.payload;
                        pushResult(resultList, temp);
                        free(temp);
                }

                if(rIndex->chain[chainPointer] != 0) {
                        chainPointer = rIndex->chain[chainPointer] - 1;
                }
                else {
                        return;
                }
        }
}

void searchKey(indexArray * indArr, headResult * resultList, tuple * checkedTuples, uint32_t tuplesNumb, uint32_t key) {
        relationIndex keyIndex = indArr->indexes[key];

        if(keyIndex.buckets == NULL) {
                return;
        }

        for(uint32_t whichTup = 0; whichTup < tuplesNumb; whichTup++) {
                followChain(resultList, &keyIndex, checkedTuples[whichTup]);
        }

}

void searchKeyRec(relationIndex * keyIndex, headResult * resultList, tuple * checkedTuples, uint32_t tuplesNumb) {
        if(keyIndex == NULL) {
                return;
        }

        for(uint32_t whichTup = 0; whichTup < tuplesNumb; whichTup++) {
                followChain(resultList, keyIndex, checkedTuples[whichTup]);
        }

        //Will uncomment when keyIndex->next is initialised properly
        if(keyIndex->next != NULL) {
                searchKeyRec(keyIndex->next, resultList, checkedTuples, tuplesNumb);
        }
}

headResult * search(indexArray * indArr, reorderedR * s, uint32_t hash2) {
        uint32_t size;
        tuple * startTup = NULL;
        uint32_t key1;
        headResult * resultList = initialiseResultHead();
        for(uint32_t whichKey = 0; whichKey < s->pSumArr.psumSize; whichKey++) {
                if(whichKey < s->pSumArr.psumSize - 1) {
                        size = s->pSumArr.psum[whichKey + 1].offset -  s->pSumArr.psum[whichKey].offset;
                }
                else {
                        size = s->rel->size - s->pSumArr.psum[whichKey].offset;
                }
                key1 = s->pSumArr.psum[whichKey].h1Res;
                startTup = &(s->rel->tuples[s->pSumArr.psum[whichKey].offset]);

                searchKeyRec(&(indArr->indexes[key1]), resultList, startTup, size);
        }

        return resultList;
}
