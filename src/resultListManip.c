#include <stdlib.h>
#include <stdio.h>

#include "basicStructs.h"
#include "resultListInterface.h"
#include "queryStructs.h"

headResult * initialiseResultHead() {
        headResult * newHead = (headResult *) malloc(sizeof(headResult));
        newHead->numbOfNodes = 0;
        newHead->buffTuple = (rowTuple *) malloc((MB / sizeof(tuple)) * sizeof(rowTuple));
        newHead->buffSize = 0;
        newHead->firstNode = NULL;
        newHead->tail = NULL;
        newHead->totalSize = 0;
        return newHead;
}

resultNode * initialiseResultNode() {
        resultNode * newNode = (resultNode *) malloc(sizeof(resultNode));
        newNode->tuples = (rowTuple *) calloc(MB, sizeof(char));
        newNode->size = 0;
        newNode->nextNode = NULL;
        return newNode;
}




void pushResult(headResult * head, rowTuple * t) {
        myint_t maxTuples = MB / sizeof(tuple);
        resultNode * finalNode = NULL;
        if(head->numbOfNodes == 0) {
                head->firstNode = initialiseResultNode();
                head->tail = head->firstNode;
                head->numbOfNodes += 1;
                finalNode = head->firstNode;
        }
        else {
                //Traversing to the end of list
                finalNode = head->firstNode;
                for(myint_t whichNode = 0; whichNode < head->numbOfNodes - 1; whichNode++) {
                        finalNode = finalNode->nextNode;
                }

                //CASE: Node is full
                if(finalNode->size == maxTuples) {
                        finalNode->nextNode = initialiseResultNode();
                        head->tail = finalNode->nextNode;
                        head->numbOfNodes += 1;
                        finalNode = finalNode->nextNode;
                }
        }

        finalNode->tuples[finalNode->size].rowR = t->rowR;
        finalNode->tuples[finalNode->size].rowS = t->rowS;
        finalNode->size += 1;
        head->totalSize += 1;

}

resultNode * initialiseResultNodeVer2() {
        resultNode * newNode = (resultNode *) malloc(sizeof(resultNode));
        newNode->nextNode = NULL;
        return newNode;
}

void pushResultVer2(headResult * head, rowTuple * t) {
        myint_t maxTuples = MB / sizeof(tuple);
        if(head->buffSize != maxTuples) {
                head->buffTuple[head->buffSize].rowR = t->rowR;
                head->buffTuple[head->buffSize].rowS = t->rowS;
                head->buffSize += 1;
        }
        else {
                if(head->numbOfNodes == 0) {
                        head->firstNode = initialiseResultNodeVer2();
                        head->tail = head->firstNode;
                }
                else {
                        head->tail->nextNode = initialiseResultNodeVer2();
                        head->tail = head->tail->nextNode;
                }

                head->numbOfNodes += 1;

                head->tail->tuples = head->buffTuple;
                head->tail->size = head->buffSize;
                head->buffTuple = (rowTuple *) malloc(maxTuples * sizeof(rowTuple));
                head->buffSize = 0;

                head->buffTuple[head->buffSize].rowR = t->rowR;
                head->buffTuple[head->buffSize].rowS = t->rowS;
                head->buffSize += 1;
        }
        head->totalSize += 1;
}

void cleanListHead(headResult * head) {
        if(head->buffSize != 0) {
                if(head->numbOfNodes != 0) {
                        head->tail->nextNode = initialiseResultNodeVer2();
                        head->tail->nextNode->tuples = head->buffTuple;
                        head->tail->nextNode->size = head->buffSize;
                }
                else {
                        head->firstNode = initialiseResultNodeVer2();
                        head->firstNode->tuples = head->buffTuple;
                        head->firstNode->size = head->buffSize;
                }
                
                head->numbOfNodes += 1;
        }
        if(head->buffSize != 0 || head->numbOfNodes != 0)
                head->buffTuple = NULL;
}

void freeResultList(headResult * head) {
        if(head->numbOfNodes != 0) {
                freeResultNode(head->firstNode);
        }
        if(head->buffTuple != NULL) {
                free(head->buffTuple);
        }
        free(head);
}

void freeResultNode(resultNode * node) {
        if(node->nextNode != NULL) {
                freeResultNode(node->nextNode);
        }
        free(node->tuples);
        free(node);
}

//It expects that rowIdS matches rowIdR (for debugging purposes)
myint_t checkResults(headResult * head) {
        resultNode * currNode;
        if(head->firstNode == NULL) {
                return 0;
        }
        currNode = head->firstNode;
        for(myint_t whichNode = 0; whichNode < head->numbOfNodes; whichNode++) {
                for(myint_t whichTup = 0; whichTup < currNode->size; whichTup++) {
                        if(currNode->tuples[whichTup].rowR != currNode->tuples[whichTup].rowS) {
                                return -1;
                        }
                }
                currNode = currNode->nextNode;
        }
        return 0;
}

//Finding the size of the results
myint_t countSizeOfList(headResult * head) {
    if(head->numbOfNodes == 0) {
        return 0;
    }

    myint_t numbOfResults = 0;
    resultNode * currentNode = head->firstNode;
    for(myint_t whichNode = 0; whichNode < head->numbOfNodes; whichNode++) {
        numbOfResults += currentNode->size;
        currentNode = currentNode->nextNode;
    }
    return numbOfResults;
}