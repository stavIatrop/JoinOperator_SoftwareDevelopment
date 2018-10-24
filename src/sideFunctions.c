#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "basicStructs.h"
#include "hashing.h"

relation * getStartOfBucket(reorderedR * ror, int whichPsum) {  		//computes and returns the start index of a bucket

	relation * newRelation = (relation *) malloc(sizeof(relation));
	int index;
	index = (ror->psum)[whichPsum].offset; 								//startIndex of specific bucket
	tuple * tempTuple;
	tempTuple =	(ror->rel).tuples + index; 
	newRelation->tuples = tempTuple;
	if(whichPsum + 1 == ror->psumSize) { 								//compute size of bucket if it is the last psum value

		newRelation->size = (ror->rel).size - (ror->psum)[whichPsum].offset; 

	} else {

		newRelation->size = (ror->psum)[whichPsum + 1].offset - (ror->psum)[whichPsum].offset;
	}

	return newRelation;
}


void updateChain(uint32_t * chain, uint32_t * buckets, int hash2Index, int i) {

	if( buckets[hash2Index] == 0 ) {		//no previous entry 		

		buckets[hash2Index] = i + 1;
		chain[i] = 0;
		//chain[i + 1] = 0;
	}else {									//reconstruct chain

		int temp;
		temp = buckets[hash2Index];
		buckets[hash2Index] = i + 1;
		chain[i] = temp;
		//chain[i + 1] = temp;
	}
	return;
}



void buildIndex(index * oneIndex, reorderedR *ror, int whichPsum, int hash2) {

	int chainSize = oneIndex->rel->size;								//chainSize is the same with the size of bucket

	int i, hash2Index;
	for (i = 0; i < chainSize; i++) {									//loop through elements of bucket

		hash2Index = hashing(oneIndex->rel->tuples[i].payload, hash2);

		updateChain(oneIndex->chain, oneIndex->buckets, hash2Index, i);

	}

	return;


}