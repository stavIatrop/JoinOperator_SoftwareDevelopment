#ifndef INTER_LIST_INTERFACE_H
#define INTER_LIST_INTERFACE_H

//Basic functions
headInter * initialiseHead();
void pushInter(headInter * head ,myint_t cols, myint_t * joinedRels, myint_t ** rowIds);
void refreshInter(nodeInter * node, myint_t cols, myint_t * joinedRels, myint_t ** rowIds);
void deleteInternode(headInter * head, nodeInter * node);
void freeInterList(headInter * head);

//Helpers (intializers, frees)
inter * initialiseInter(myint_t cols, myint_t * joinedRels, myint_t ** rowIds);
nodeInter * initialiseNode(myint_t cols, myint_t * joinedRels, myint_t ** rowIds);
void freeNode(nodeInter * node);
void freeNodeListRec(nodeInter * node);




#endif