#ifndef RESULT_LIST_INTERFACE_H
#define RESULT_LIST_INTERFACE_H

#include "basicStructs.h"
#include "queryStructs.h"

#define MB 1048576

//Updated result list: The head of the list has a 1MB buffer of rowTuples(results) and a tail pointer. 
//When the buffer is full, we save it in a new node (taile->next) and create a new buffer in the head.
//This way we achieve an O(1) push.

headResult * initialiseResultHead();
headResult * initialiseResultHeadNoBuff();
resultNode * initialiseResultNode();
resultNode * initialiseResultNodeVer2();

void pushResult(headResult * head, rowTuple * t);

void pushResultVer2(headResult * head, rowTuple * t);
void cleanListHead(headResult * head);

void freeResultList(headResult * head);
void freeResultNode(resultNode * node);

myint_t checkResults(headResult * head);
myint_t countSizeOfList(headResult * head);

#endif