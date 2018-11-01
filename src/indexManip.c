#include <stdio.h>
#include <stdlib.h>
#include "basicStructs.h"


relationIndex initializeIndex(uint32_t bucketSize, relation * rel, int32_t key, relationIndex * next, uint32_t hash2) {

	relationIndex newIndex;
	newIndex.key = key;
	newIndex.rel = rel;
	newIndex.hash2 = hash2;
	newIndex.next = next; 
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

void RecFree(relationIndex * indexes) {

	relationIndex * temp =  indexes;
		
	while (temp->next != NULL) {

		RecFree(temp->next);
	}

	free(temp->chain);
	free(temp->buckets);
	free(temp->rel);
	free(temp);

	return;
}


void freeIndexArray(indexArray * idxArray) {

	uint32_t i;
	for (i = 0; i < idxArray->size; i++){

		if(idxArray->indexes[i].next != NULL) {		//because first nodes of every index are not dynamically allocated
			
			RecFree(idxArray->indexes[i].next);

		}else {

			free(idxArray->indexes[i].chain);
			free(idxArray->indexes[i].buckets);
			free(idxArray->indexes[i].rel);
		}
		
	}
	
	free(idxArray->indexes);
	free(idxArray);

	return;
}