#ifndef BASIC_STRUCTS_H
#define BASIC_STRUCTS_H

#include <stdint.h>

#define CACHE_SIZE (2<<15)
#define AVAILABLE_CACHE_SIZE (CACHE_SIZE * 20 / 21)
#define ERROR_MARGIN 1.05
#define BUCKET_MEMORY_LIMIT (2<<28)
#define FIRST_REORDERED 100

typedef uint64_t myint_t;

typedef struct Tuple {
        myint_t key;
        myint_t payload;
} tuple;

typedef struct Relation {
        tuple * tuples;
        myint_t size;
        myint_t dvalues;
} relation;

//<-- ReorderedR structs
typedef struct HistTuple {
        myint_t h1Res;
        myint_t offset;
} histTuple;

typedef struct PSumTuple {
        myint_t h1Res;
        myint_t offset;
} pSumTuple;

typedef struct PSumArray {
        pSumTuple * psum;
        myint_t psumSize;
}pSumArray;

typedef struct ReorderedR {
        pSumArray pSumArr;
        relation * rel;
} reorderedR;
// -->

//<-- Index structs
typedef struct RelationIndex {
        myint_t * chain;
        myint_t * buckets;
        relation * rel;
        myint_t key;
        struct RelationIndex * next;
        myint_t hash2;
} relationIndex;

typedef struct IndexArray {
       relationIndex * indexes;
       myint_t size;
} indexArray;
//-->

//<-- Result list structs
typedef struct ResultNode resultNode;

typedef struct RowTuple {
        myint_t rowR;
        myint_t rowS;
} rowTuple;

struct ResultNode {
        rowTuple * tuples;
        myint_t size;
        resultNode * nextNode;
};

typedef struct HeadResult {
        resultNode * firstNode;
        resultNode * tail;
        myint_t numbOfNodes;
        myint_t totalSize;
        rowTuple * buffTuple;
        myint_t buffSize;
} headResult;
//-->

//Valerios
reorderedR * reorderRelation(relation * r, myint_t *hash1);

//Stavroula
indexArray * indexing(reorderedR * ror, myint_t hash1);

//Mixalhs
headResult * search(indexArray * indArr, reorderedR * s);
headResult * searchThreadVer(indexArray * indArr, reorderedR * s);

#endif
