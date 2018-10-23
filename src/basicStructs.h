#ifndef BASIC_STRUCTS_H
#define BASIC_STRUCTS_H

#include <stdint.h>

typedef struct Tuple {
        int32_t key;
        int32_t payload;
} tuple;

typedef struct Relation {
        tuple * tuples;
        uint32_t size;
} relation;

typedef struct PSumTuple {
        int32_t h1Res;
        int32_t offset;
} pSumTuple;

typedef struct ReorderedR {
        pSumTuple * psum;
        relation rel;
} reorderedR;

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


//Valerios
reorderedR * reordereRelation(relation * r, int hash1);

//Stavroula
indexArray * indexing(reorderedR * ror, int hash2);

//Kalyteros
headResult * search(indexArray indexes, reorderedR * s, int hash2);




#endif