#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "basicStructs.h"
#include "inputInterface.h"
#include "indexManip.h"
#include "viceFunctions.h"

headResult * radixHashJoin(table * rTable, table * sTable, int32_t colR, int32_t colS) {

        //TO BE REMOVED
        /*relation * rRel = malloc(sizeof(relation));
        relation * sRel = malloc(sizeof(relation));
        int size1 = 10000000, size2 = 10000000;
	rRel->tuples = malloc(size1 * sizeof(tuple));
	rRel->size=size1;
        sRel->tuples = malloc(size2 * sizeof(tuple));
	sRel->size=size2;
	uint32_t random;

	srand(time(NULL));
        for (uint32_t i=0; i<size1; i++)
        {
                random = rand();
                rRel->tuples[i].key = random;
                rRel->tuples[i].payload = 837376;
                //r->tuples[i].key = 5;
                //r->tuples[i].payload = 837376;
        }

        for (uint32_t i=0; i<size2; i++)
        {
                random = rand();
                sRel->tuples[i].key = random;
                sRel->tuples[i].payload = 837376;
                //s->tuples[i].key = i;
                //s->tuples[i].payload = i;
        }*/

        relation * rRel = extractRelation(rTable, colR);
        relation * sRel = extractRelation(sTable, colS);
        double radixTotalTime = 0;

        uint32_t h1 = FIRST_REORDERED;
        printf(">>> Starting Reordering...");
        clock_t begin = clock();
        reorderedR * RoR;
        reorderedR * RoS;
	double rIdenticality = IdenticalityTest(rRel);
	double sIdenticality = IdenticalityTest(sRel);

	if (abs(rRel->size - sRel->size) < 100000 || abs(rRel->size - sRel->size) > 10000000)
	{
		if ((rRel->size)*(1-sqrt(rIdenticality)) <= (sRel->size)*(1-sqrt(sIdenticality)))
		{
			RoR = reorderRelation(rRel, &h1);
			RoS = reorderRelation(sRel, &h1);
		}
		else
		{
			RoR = reorderRelation(sRel, &h1);
        	        RoS = reorderRelation(rRel, &h1);
		}
	}
	else
	{
		if ((rRel->size)*(1-sqrt(sIdenticality)) >= (sRel->size)*(1-sqrt(rIdenticality)))
                {
                        RoR = reorderRelation(rRel, &h1);
                        RoS = reorderRelation(sRel, &h1);
                }
                else
                {
                        RoR = reorderRelation(sRel, &h1);
                        RoS = reorderRelation(rRel, &h1);
                }

	}
        clock_t end = clock();
        radixTotalTime += (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Completed in  %f seconds.\n", (double)(end - begin) / CLOCKS_PER_SEC);

        //Indexing
        printf(">>> Starting Indexing...  ");
        begin = clock();
        indexArray * indArr = indexing(RoR, h1);
        end = clock();
        radixTotalTime += (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Completed in  %f seconds.\n", (double)(end - begin) / CLOCKS_PER_SEC);

        //Searching
        printf(">>> Starting Searching... ");
        begin = clock();
        headResult * results = search(indArr, RoS);
        end = clock();
        radixTotalTime += (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Completed in  %f seconds.\n", (double)(end - begin) / CLOCKS_PER_SEC);

        printf(">>> Radix Hash Join completed in %f seconds.\n", radixTotalTime);

        free(rRel->tuples);
        free(sRel->tuples);
        free(rRel);
        free(sRel);
        free(RoR->pSumArr.psum);
        free(RoR);
        free(RoS->pSumArr.psum);
        free(RoS);

        freeIndexArray(indArr);
        
        return results;
}
