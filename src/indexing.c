#include <stdio.h>
#include "sideFunctions.h"
#include "hashing.h"
#include "indexManip.h"

indexArray * indexing(reorderedR * ror, int hash1, int hash2) {

	int sizeOfIndex = hash1Range(hash1);
	indexArray * mainIndexArray = initializeIndexArray(sizeOfIndex);

	int i, whichPsum = 0;
	for( i = 0; i < sizeOfIndex; i++) {

		if(i == (ror->pSumArr).psum[whichPsum].h1Res){ 			//index is created when there is a bucket of i's

			relation * rel = getStartOfBucket(ror, whichPsum);
			int bucketArrSize = hash2Range(hash2);
			mainIndexArray->indexes[i] = initializeIndex(bucketArrSize, rel, i);

			buildIndex(mainIndexArray->indexes + i, ror, i, whichPsum, hash2);  //Mporei na allaxei epeidh den einai synexomena(?)
			whichPsum++;
			
		}else { 												//otherwise no index is created for bucket of i's 

			mainIndexArray->indexes[i] = initializeIndex(0, NULL, i);
		}
	}

	return mainIndexArray;

}