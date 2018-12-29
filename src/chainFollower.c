#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "basicStructs.h"
#include "resultListInterface.h"
#include "hashing.h"
#include "chainFollowerInterface.h"
#include "jobScheduler.h"
#include "pthread.h"

pthread_mutex_t finalResultListLock;
int connectedLists;
pthread_cond_t cond_finished_search;
pthread_mutex_t finished_search_mutex;

void followChain(headResult * resultList, relationIndex * rIndex, tuple t, myint_t h1) {
        myint_t hashRes = hashing(t.key, h1, rIndex->hash2);
        myint_t chainPointer = rIndex->buckets[hashRes] - 1;

        //Case: No matching h2
        if(chainPointer == EMPTY_BUCKET) {
                return;
        }
        while(1) {
                //Check if the actual values match (not only their h2 results)
                if(rIndex->rel->tuples[chainPointer].key == t.key) {
                        //printf("RowId = ")
                        rowTuple * temp = (rowTuple *) malloc(sizeof(rowTuple));
                        temp->rowR = rIndex->rel->tuples[chainPointer].payload;
                        temp->rowS = t.payload;
                        //pushResult(resultList, temp);
                        pushResultVer2(resultList, temp);
                        //counter++;
                        free(temp);
                }

                //Chain has not ended
                if(rIndex->chain[chainPointer] != 0) {
                        chainPointer = rIndex->chain[chainPointer] - 1;
                }
                else {
                        //We have reached the end of chain
                        return;
                }
        }
}

void searchKeyRec(relationIndex * keyIndex, headResult * resultList, tuple * checkedTuples, myint_t tuplesNumb, myint_t h1) {
        //Empty index (no matching h2 value)
        if(keyIndex->rel == NULL) {
                return;
        }

        for(myint_t whichTup = 0; whichTup < tuplesNumb; whichTup++) {
                followChain(resultList, keyIndex, checkedTuples[whichTup], h1);
        }

        //Follow "sub buckets" if they exist
        if(keyIndex->next != NULL) {
                searchKeyRec(keyIndex->next, resultList, checkedTuples, tuplesNumb, h1);
        }
}

headResult * search(indexArray * indArr, reorderedR * s) {
        myint_t size;
        tuple * startTup = NULL;
        myint_t key1;
        headResult * resultList = initialiseResultHead();
        for(myint_t whichKey = 0; whichKey < s->pSumArr.psumSize; whichKey++) {
                //Variable size is how many values belong in the same h1 key
                if(whichKey < s->pSumArr.psumSize - 1) {
                        size = s->pSumArr.psum[whichKey + 1].offset -  s->pSumArr.psum[whichKey].offset;
                }
                else {
                        size = s->rel->size - s->pSumArr.psum[whichKey].offset;
                }
                key1 = s->pSumArr.psum[whichKey].h1Res;
                
                startTup = &(s->rel->tuples[s->pSumArr.psum[whichKey].offset]);

                searchKeyRec(&(indArr->indexes[key1]), resultList, startTup, size, (myint_t) log2(indArr->size));
        }

        //fprintf(stderr, "PUSHES: %ld\n", counter);
        cleanListHead(resultList);
        return resultList;
}

void connectResultList(headResult * finalResult, headResult * joinedList) {
        if(joinedList->totalSize != 0) {
                pthread_mutex_lock(&finalResultListLock);
                if(finalResult->totalSize == 0) {
                        finalResult->firstNode = joinedList->firstNode;
                }

                finalResult->totalSize += joinedList->totalSize;
                finalResult->numbOfNodes += joinedList->numbOfNodes;
                finalResult->tail = joinedList->tail;
                pthread_mutex_unlock(&finalResultListLock);
        }
        
}

void joinBucket(void * args) {
        searchArgument * funcArgs = (searchArgument *) args;
        headResult * resultList = initialiseResultHead();
        
        searchKeyRec(&(funcArgs->indArr->indexes[funcArgs->key1]), resultList, funcArgs->startTup, funcArgs->size, (myint_t) log2(funcArgs->indArr->size));

        cleanListHead(resultList);

        connectResultList(funcArgs->finalResult, resultList);

        free(resultList);
        

        pthread_mutex_lock(&finished_search_mutex);
        connectedLists += 1;
        if(connectedLists == funcArgs->sBuckets) {
                fprintf(stderr, "se xypnao?");
                pthread_cond_signal(&cond_finished_search);
        }
        pthread_mutex_unlock(&finished_search_mutex);
        free(args);
}

headResult * searchThreadVer(indexArray * indArr, reorderedR * s) {
        pthread_mutex_init(&finalResultListLock, NULL);
        pthread_mutex_init(&finished_search_mutex, 0);
        pthread_cond_init(&cond_finished_search, 0);
        connectedLists = 0;
        myint_t size;
        tuple * startTup = NULL;
        myint_t key1;
        headResult * finalResultList = initialiseResultHeadNoBuff();
        for(myint_t whichKey = 0; whichKey < s->pSumArr.psumSize; whichKey++) {
                //Variable size is how many values belong in the same h1 key
                if(whichKey < s->pSumArr.psumSize - 1) {
                        size = s->pSumArr.psum[whichKey + 1].offset -  s->pSumArr.psum[whichKey].offset;
                }
                else {
                        size = s->rel->size - s->pSumArr.psum[whichKey].offset;
                }
                key1 = s->pSumArr.psum[whichKey].h1Res;
                
                startTup = &(s->rel->tuples[s->pSumArr.psum[whichKey].offset]);
                
                //Initialise arguments of job
                searchArgument * sArgs = (searchArgument *) malloc(sizeof(searchArgument));
                sArgs->finalResult = finalResultList;
                sArgs->indArr = indArr;
                sArgs->key1 = key1;
                sArgs->size = size;
                sArgs->startTup = startTup;
                sArgs->sBuckets = s->pSumArr.psumSize;

                //searchKeyRec(&(indArr->indexes[key1]), resultList, startTup, size, (myint_t) log2(indArr->size));
                //Initialise job
                struct Job * job = (struct Job *) malloc(sizeof(struct Job));
                job->function = &joinBucket;
                job->argument = (void *) sArgs;

                writeOnQueue(job);
        }

        pthread_mutex_lock(&finished_search_mutex);
        fprintf(stderr, "SIZE = %ld\n", s->pSumArr.psumSize);
        while(connectedLists != s->pSumArr.psumSize) {
                //fprintf(stderr, "CCCCCC\n");
                pthread_cond_wait(&cond_finished_search, &finished_search_mutex);
        }
        pthread_mutex_unlock(&finished_search_mutex);

        fprintf(stderr, "AAAAAAAAAAAAA\n");
        //fprintf(stderr, "PUSHES: %ld\n", counter);
        //cleanListHead(resultList);
        pthread_mutex_destroy(&finalResultListLock);
        pthread_cond_destroy(&cond_finished_search);
        pthread_mutex_destroy(&finished_search_mutex);

        return finalResultList;
}

