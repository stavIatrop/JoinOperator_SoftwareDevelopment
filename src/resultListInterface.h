#ifndef RESULT_LIST_INTERFACE_H
#define RESULT_LIST_INTERFACE_H

#include "basicStructs.h"

#define MB 1048576

headResult * initialiseResultHead();
resultNode * initialiseResultNode();
void pushResult(headResult * head, tuple * t);
void freeResultList(headResult * head);
void freeResultNode(resultNode * node);

#endif