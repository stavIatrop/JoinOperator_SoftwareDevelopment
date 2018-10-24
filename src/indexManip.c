#include <stdio.h>
#include "basicStructs.h"


index initializeIndex(int bucketSize, relation * rel, int key) {

	index newIndex;
	newIndex.key = key;
	newIndex.rel = rel; 
	if (rel == NULL) {
		newIndex.chain = NULL;
		newIndex.bucket = NULL;
		return newIndex;
	}
	newIndex->chain = (uint32_t *) malloc(rel->size * sizeof(uint32_t));
	newIndex->bucket = (uint32_t *) malloc(bucketSize * sizeof(uint32_t));
	return newIndex;

}

indexArray * initializeIndexArray(int size){

	indexArray * newIndexArray = (indexArray *) malloc(sizeof(indexArray));
	newIndexArray->size = size;
	newIndexArray->indexes = (index *) malloc(size * sizeof(index));
	return indexArray;

}

void freeIndexArray(indexArray * idxArray) {

	int i;
	for (i = 0; i < idxArray->size; i++){

		free((idxArray->indexes)[i].chain);
		free((idxArray->indexes)[i].bucket);
	}
	
	free(idxArray->indexes);
	free(idxArray);

	return;
}