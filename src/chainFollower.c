#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "basicStructs.h"
#include "resultListInterface.h"
#include "hashing.h"
#include "chainFollowerInterface.h"

void followChain(headResult * resultList, relationIndex * rIndex, tuple * t, int h2) {
        uint32_t hashRes = performHash(t->payload, h2);
        uint32_t chainPointer = rIndex->buckets[hashRes];
        //Case: No matching h2
        if(chainPointer == 0) {
                return;
        }
        while(1) {
                if(rIndex->rel->tuples[chainPointer - 1].payload == t->payload) {
                        tuple * temp = (tuple *) malloc(sizeof(tuple));
                        temp->key = rIndex->rel->tuples[chainPointer - 1].key;
                        temp->payload = t->key;
                        pushResult(resultList, temp);
                        free(temp);
                }

                if(rIndex->chain[chainPointer] != 0) {
                        chainPointer = rIndex->chain[chainPointer];
                }
                else {
                        return;
                }
        }
}