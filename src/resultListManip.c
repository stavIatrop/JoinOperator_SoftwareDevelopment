#include <stdlib.h>
#include <stdio.h>

#include "basicStructs.h"
#include "resultListInterface.h"
#include "queryStructs.h"

headResult * initialiseResultHead() {
        headResult * newHead = (headResult *) malloc(sizeof(headResult));
        newHead->numbOfNodes = 0;
        newHead->firstNode = NULL;
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
                        head->numbOfNodes += 1;
                        finalNode = finalNode->nextNode;
                }
        }

        finalNode->tuples[finalNode->size].rowR = t->rowR;
        finalNode->tuples[finalNode->size].rowS = t->rowS;
        finalNode->size += 1;

}

void freeResultList(headResult * head) {
        if(head->numbOfNodes != 0) {
                freeResultNode(head->firstNode);
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