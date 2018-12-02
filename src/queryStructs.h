#ifndef QUERY_STRUCTS_H
#define QUERY_STRUCTS_H

#include <stdint.h>
#include "basicStructs.h"

#define LESS 1
#define GREATER 2
#define EQUAL 3

typedef uint64_t myint_t;

typedef struct Relationsheep {
        myint_t rows;
        myint_t cols;
        myint_t **pointToCols;
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
        myint_t rel;
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
} join;

typedef struct Query {
        myint_t *rels;
        myint_t numOfFilters;
        myint_t numOfJoins;
        myint_t numOfSums;
        filter *filters;
        join *joins;
        colRel *sums;
} query;

#endif
