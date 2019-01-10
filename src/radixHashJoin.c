#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "basicStructs.h"
#include "inputInterface.h"
#include "indexManip.h"
#include "viceFunctions.h"

headResult * radixHashJoin(relation * rRel, relation * sRel, char * switched) {

        myint_t h1 = FIRST_REORDERED;
        reorderedR * RoR;
        reorderedR * RoS;
	double rIdenticality = IdenticalityTest(rRel);
	double sIdenticality = IdenticalityTest(sRel);
                
        //Reordering (Choosing best relationship to create the index)
	if ((rRel->size)*rIdenticality <= (sRel->size)*sIdenticality)
	{
		RoR = reorderRelation(rRel, &h1);
                //fprintf(stderr, "MMM\n");
		RoS = reorderRelation(sRel, &h1);
                *switched = 0;
	}
	else
	{
		RoR = reorderRelation(sRel, &h1);
                //fprintf(stderr, "MMM\n");
	        RoS = reorderRelation(rRel, &h1);
                *switched = 1;
	}
        //fprintf(stderr, "LLL\n");


        //Indexing
        indexArray * indArr = indexing(RoR, h1);

        //Searching
        headResult * results = searchThreadVer(indArr, RoS);

        free(RoR->rel->tuples);
        free(RoR->rel);
        free(RoS->rel->tuples);
        free(RoS->rel);
        free(RoR->pSumArr.psum);
        free(RoR);
        free(RoS->pSumArr.psum);
        free(RoS);
        
        freeIndexArray(indArr);
        
        return results;
}
