#ifndef GET_STATS_H
#define GET_STATS_H

#define N 50000000

#include "queryStructs.h"

void FillStatsArray(myint_t * , stats * , myint_t  );
void PredicateSequence(query * , relationsheepArray );

#endif