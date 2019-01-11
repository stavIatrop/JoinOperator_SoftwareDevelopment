#ifndef JOIN_ENUMERATION_H
#define JOIN_ENUMERATION_H

#include "hashTreeManip.h"
#include "graph.h"

HTNode * CreateJoinTree(HTNode * , HTNode *  ,char * , myint_t, myint_t, myint_t);
int existsInComb(char * , myint_t );
int areConnected(char *, myint_t, graph *);
void joinEnumeration(HTNode **, query *, relationsheepArray, graph * );

#endif