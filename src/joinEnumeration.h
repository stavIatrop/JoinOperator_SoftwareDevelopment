#ifndef JOIN_ENUMERATION_H
#define JOIN_ENUMERATION_H

#include "hashTreeManip.h"
#include "graph.h"

void Filter(HTStats * , myint_t , myint_t , myint_t );
void Join( HTStats * , HTStats * , myint_t , myint_t );
HTNode * CreateJoinTree(HTNode * , HTNode *  ,char * , myint_t, myint_t, myint_t);
int existsInComb(char * , myint_t );
int areConnected(char *, myint_t, graph *, myint_t *, myint_t *, myint_t *);
void joinEnumeration(HTNode **, query *, relationsheepArray, graph * );
void SetPriorities(char * , query * );

#endif