#include <stdio.h>
#include "basicStructs.h"

relation * getStartOfBucket(reorderedR * ror, int whichPsum) {  //computes and returns the start index of a bucket

	relation * newRelation = (relation *) malloc(sizeof(relation));
	int index;
	index = (ror->psum)[whichPsum].offset; //startIndex of specific bucket
	tuple * tempTuple;
	tempTuple =	(ror->rel).tuples + index; 
	newRelation->tuples = tempTuple;
	if(whichPsum + 1 == ror->psumSize) { //compute size of bucket if it is the last psum value

		newRelation->size = (ror->rel).size - (ror->psum)[whichPsum].offset; 

	} else {

		newRelation->size = (ror->psum)[whichPsum + 1].offset - (ror->psum)[whichPsum].offset;
	}

	return newRelation;
}

void buildIndex(indexArray * inAr, reorderedR *ror, int i, int whichPsum, int hash2) {

	
}