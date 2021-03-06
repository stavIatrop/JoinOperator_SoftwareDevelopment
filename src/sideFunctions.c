#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "basicStructs.h"
#include "indexManip.h"
#include "hashing.h"

relation * getStartOfBucket(reorderedR * ror, myint_t whichPsum) {  		//computes and returns the start index of a bucket

	relation * newRelation = (relation *) malloc(sizeof(relation));
	myint_t index;
	index = ror->pSumArr.psum[whichPsum].offset; 								//startIndex of specific bucket
	tuple * tempTuple;
	tempTuple =	&(ror->rel->tuples[index]); 
	newRelation->tuples = tempTuple;
	if(whichPsum + 1 == ror->pSumArr.psumSize ) { 					//compute size of bucket if it is the last psum value

		newRelation->size = ror->rel->size - ror->pSumArr.psum[whichPsum].offset; 

	} else {

		newRelation->size = ror->pSumArr.psum[whichPsum + 1].offset - ror->pSumArr.psum[whichPsum].offset;
	}

	return newRelation;
}


void updateChain(myint_t * chain, myint_t * buckets, myint_t hash2Index, myint_t i) {

	myint_t temp;
	temp = buckets[hash2Index];
	buckets[hash2Index] = i + 1;
	chain[i] = temp;

	return;
}



void buildIndex(relationIndex * oneIndex, myint_t hash1, myint_t hash2) {

	myint_t chainSize = oneIndex->rel->size;							//chainSize is the same with the size of bucket

	myint_t i, hash2Index;
	for (i = 0; i < chainSize; i++) {									//loop through elements of bucket

		hash2Index = hashing(oneIndex->rel->tuples[i].key, hash1, hash2);
		updateChain(oneIndex->chain, oneIndex->buckets, hash2Index, i);

	}

	return;

}


relation * getStartOfSubBucket(tuple *startOfBuck, myint_t sizeIndexedSofar, myint_t eachSize) {

	relation * newRelation = (relation *) malloc(sizeof(relation));
	newRelation->size = eachSize;
	tuple * tempTuple;
	tempTuple =	&(startOfBuck[sizeIndexedSofar]); 
	newRelation->tuples = tempTuple;
	
	return newRelation;
}

//when this function is called, indexing has already construct head of indexingList
void buildSubIndex(relationIndex ** oneIndex, myint_t hash1, myint_t hash2, myint_t sizeAll, myint_t eachSize, myint_t sizeIndexedSofar, tuple * startOfBuck, myint_t i) {

	
	myint_t bucketSize = hash2Range(hash2);
	

	while(sizeAll > 0) {

		if( sizeAll < eachSize) {
			eachSize = sizeAll;
		}

		relation * rel = getStartOfSubBucket(startOfBuck, sizeIndexedSofar, eachSize);

		
		*oneIndex = (relationIndex *) malloc(sizeof(relationIndex));
		**oneIndex = initializeIndex(bucketSize, rel, i, NULL, hash2);
		buildIndex( *oneIndex, hash1, hash2);
		oneIndex = &((*oneIndex)->next);
		sizeIndexedSofar += eachSize;
		sizeAll -= eachSize;
	}
	return;
}


_Bool IsPowerOfTwo(myint_t x)
{
	return ((x & (x - 1)) == 0);
}

myint_t findPowerOf2Hash(myint_t range) {

	myint_t flag = 0;
	if (range == 1)
		return 0;
	while(flag == 0 ) {

		if (IsPowerOfTwo(range)) {
			flag =1;
			break;
		}
		range--;

	}

	return range;
}


void recomputeSizes(myint_t * eachSize, myint_t * hash2Var, myint_t sizeAll) {

	*eachSize = sizeAll/2;

	myint_t sizeOfIndex = sizeof(myint_t) + (*eachSize) * ( sizeof(tuple) + sizeof(myint_t) ) + hash2Range(*hash2Var) * sizeof(myint_t);

	while( sizeOfIndex > CACHE_SIZE ) {

		(*eachSize) = (*eachSize) / 2;
		sizeOfIndex = sizeof(myint_t) + (*eachSize) * ( sizeof(tuple) + sizeof(myint_t) ) + hash2Range(*hash2Var) * sizeof(myint_t);

	}

	while (sizeOfIndex <= CACHE_SIZE) {

		(*hash2Var) = (*hash2Var) + 1;
		sizeOfIndex = sizeof(myint_t) + (*eachSize) * ( sizeof(tuple) + sizeof(myint_t) ) + hash2Range(*hash2Var) * sizeof(myint_t);
	}

	(*hash2Var) = (*hash2Var) - 1;
	return;

}



