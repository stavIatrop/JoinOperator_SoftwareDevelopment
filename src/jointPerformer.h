#ifndef JOINT_PERFORMER

#define JOINT_PERFORMER
#include <queryStructs.h>
#include <basicStructs.h>

void workerJ(join*,headInter*);
myint_t findNextRowId(myint_t *,myint_t,myint_t);
relation *forgeRelationsheep(headInter *,colRel *);
headResult *performRHJ(headInter *, colRel *, colRel *, myint_t *);
headResult *performSameNodeJoin(nodeInter *, join *, myint_t *);
headResult *performSelfJoin(nodeInter *, join *, myint_t *);
//char inInterList(headinter *, myint_t, myint_t);
nodeInter *findNode(headInter *, myint_t);

#endif
