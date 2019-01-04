#ifndef INTER_LIST_INTERFACE_H
#define INTER_LIST_INTERFACE_H

#include "queryStructs.h"

//Join results Intermediate manipulation. They are responsible for operations concerning Intermediates
void createInterFromRes(headInter * headInt, headResult * headRes, myint_t rel1, myint_t rel2, char switched);
void updateInterFromRes(nodeInter * intNode, headResult * headRes, myint_t addedRel, myint_t existingRel, myint_t skipped, char switched);
void updateInterAndDelete(headInter * headInt, nodeInter * node1, nodeInter * node2, headResult * headRes, myint_t, myint_t, myint_t, char switched);
void updateInterSelfJoin(nodeInter * node, myint_t * joinRows, myint_t numbOfRows);
void createInterSelfJoin(headInter * head, myint_t rel, myint_t * rows, myint_t numbOfRows);


//Basic Intermediate List functions
headInter * initialiseHead();
void pushInter(headInter * head ,myint_t cols, myint_t rows, myint_t * joinedRels, myint_t ** rowIds);
void updateInter(nodeInter * node, myint_t cols, myint_t rows, myint_t * joinedRels, myint_t ** rowIds);
void deleteInterNode(headInter * head, nodeInter * node);
void freeInterList(headInter * head);

//Helpers (intializers, frees, etc)
inter * initialiseInter(myint_t cols, myint_t rows, myint_t * joinedRels, myint_t ** rowIds);
nodeInter * initialiseNode(myint_t cols, myint_t rows, myint_t * joinedRels, myint_t ** rowIds);
void freeNode(nodeInter * node);
void freeNodeListRec(nodeInter * node);

myint_t ** createResultArray(headResult * head, myint_t * size, char switched);
myint_t ** updateRowIds(nodeInter * intNode, headResult * headRes, myint_t *results, myint_t existingRel, char switched);
myint_t ** joinRowIds(nodeInter * node1, nodeInter * node2, headResult * headRes, myint_t *results, char switched);


#endif