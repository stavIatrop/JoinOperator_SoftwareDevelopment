#include <stdio.h>
#include <stdlib.h>
#include "basicStructs.h"


relationIndex initializeIndex(uint32_t bucketSize, relation * rel, int32_t key) {

	relationIndex newIndex;
	newIndex.key = key;
	newIndex.rel = rel; 
	if (rel == NULL) {
		newIndex.chain = NULL;
		newIndex.buckets = NULL;
		return newIndex;
	}
	newIndex.chain = (uint32_t *) malloc(rel->size * sizeof(uint32_t));
	uint32_t i;
	for( i = 0; i < rel->size; i++)
		newIndex.chain[i] = 0;

	newIndex.buckets = (uint32_t *) malloc(bucketSize * sizeof(uint32_t));
	for( i = 0; i < bucketSize; i++)
		newIndex.buckets[i] = 0;

	return newIndex;

}

indexArray * initializeIndexArray(uint32_t size){

	indexArray * newIndexArray = (indexArray *) malloc(sizeof(indexArray));
	newIndexArray->size = size;
	newIndexArray->indexes = (relationIndex *) malloc(size * sizeof(relationIndex));
	return newIndexArray;

}

void freeIndexArray(indexArray * idxArray) {

	uint32_t i;
	for (i = 0; i < idxArray->size; i++){

		free((idxArray->indexes)[i].chain);
		free((idxArray->indexes)[i].buckets);
	}
	
	free(idxArray->indexes);
	free(idxArray);

	return;
}