#ifndef RESULT_LIST_INTERFACE_H
#define RESULT_LIST_INTERFACE_H

#include "basicStructs.h"
#include "queryStructs.h"

#define MB 1048576

headResult * initialiseResultHead();
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