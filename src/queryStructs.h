#ifndef QUERY_STRUCTS_H
#define QUERY_STRUCTS_H

#include <stdint.h>
#include "basicStructs.h"

#define LESS 1
#define GREATER 2
#define EQUAL 3

typedef struct StatisticsStruct {

        myint_t minI;
        myint_t maxU;
        myint_t numElements;
        myint_t distinctVals;
        myint_t * distinctArray;

} stats;


typedef struct Wares {
        myint_t *rel;
        myint_t *col;
        myint_t *hash1;
        indexArray **indexes;
        myint_t size;
} wares;

typedef struct ReorderedWares {
        myint_t *rel;
        myint_t *col;
        myint_t *hash1;
        reorderedR **arrays;
        reorderedR ***bestHashValues;
        myint_t size;
} reWares;

wares *indexWarehouse;
reWares *reWarehouse;

typedef struct Relationsheep {

        myint_t rows;
        myint_t cols;
        myint_t **pointToCols;
        stats * statsArray;             //permanent struct for query statistics for each column
                
} relationsheep;

typedef struct RelationsheepArray {
        relationsheep *rels;
        myint_t numOfRels;
} relationsheepArray;

typedef struct Inter {
        myint_t numOfCols;
        myint_t numbOfRows;
        myint_t **rowIds;
        myint_t *joinedRels;
} inter;

typedef struct NodeInter nodeInter;
struct NodeInter {
        inter *data;
        nodeInter *next;
} ;

typedef struct HeadInter {
        myint_t numOfIntermediates;
        nodeInter *start;
} headInter;

typedef struct ColRel {
        myint_t realRel;
        myint_t realCol;
        myint_t rel;
        myint_t numCol;
        myint_t* col;
        myint_t rows;
} colRel;

typedef struct Filter {
        colRel participant;
        char op;
        myint_t value;
} filter;

typedef struct Join {
        colRel participant1;
        colRel participant2;
        myint_t distinctVals;
} join;

typedef struct Query {
        myint_t *rels;
        stats ** queryStats;             //temporary struct for query statistics for each column for each query
        myint_t numOfFilters;
        myint_t numOfJoins;
        myint_t numOfSums;
        myint_t numOfRels;
        filter *filters;
        join *joins;
        colRel *sums;
        myint_t * priorities;
} query;

#endif
