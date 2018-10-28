#ifndef BASIC_STRUCTS_H
#define BASIC_STRUCTS_H

#include <stdint.h>

typedef struct Tuple {
        int32_t key;
        uint32_t payload;
} tuple;

typedef struct Relation {
        tuple * tuples;
        uint32_t size;
} relation;

//<-- ReorderedR structs
typedef struct HistTuple {
        uint32_t h1Res;
        uint32_t offset;
} histTuple;

typedef struct PSumTuple {
        uint32_t h1Res;
        uint32_t offset;
} pSumTuple;

typedef struct PSumArray {
        pSumTuple * psum;
        uint32_t psumSize;
}pSumArray;

typedef struct ReorderedR {
        pSumArray pSumArr;
        relation * rel;
} reorderedR;
// -->

//<-- Index structs
typedef struct RelationIndex {
       uint32_t * chain;
       uint32_t * buckets;
       relation * rel;
       int32_t key;
} relationIndex;

typedef struct IndexArray {
       relationIndex * indexes;
       uint32_t size;
} indexArray;
//-->

//<-- Result list structs
typedef struct ResultNode resultNode;

struct ResultNode {
        tuple * tuples;
        uint32_t size;
        resultNode * nextNode;
};

typedef struct HeadResult {
        resultNode * firstNode;
        uint32_t numbOfNodes;
} headResult;
//-->

//Valerios
reorderedR * reordereRelation(relation * r, uint32_t *hash1);

//Stavroula
indexArray * indexing(reorderedR * ror, uint32_t hash1, uint32_t hash2);

//Kalyteros
headResult * search(indexArray indArr, reorderedR * s, uint32_t hash2);

#endif