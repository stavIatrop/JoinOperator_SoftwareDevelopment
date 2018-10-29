#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "basicStructs.h"
#include "viceFunctions.h"
#include "resultListInterface.h"
#include "chainFollowerInterface.h"
#include "sideFunctions.h"
#include "hashing.h"
#include "indexManip.h"

int main(int argc, char const *argv[])
{
        uint32_t hash1=0, size = 10000000, size2 = 20000000;
	relation *r, *s;
        r = (relation *) malloc(sizeof(relation));
        s = (relation *) malloc(sizeof(relation));

	r->tuples = malloc(size * sizeof(tuple));
        s->tuples = malloc(size2 * sizeof(tuple));
	r->size=size;
        s->size=size2;
	uint32_t random;
	srand(time(NULL));
	reorderedR *R, *S;

	for (uint32_t i=0; i<size; i++)
        {
                random = rand();
                r->tuples[i].key = i;
                r->tuples[i].payload = i;
        }
        printf("Entering reordering function\n");
        R = reorderRelation(r,&hash1);
        printf("Exited reordering function\n");

        for (uint32_t i=0; i<size2; i++)
        {
                random = rand();
                s->tuples[i].key = i;
                s->tuples[i].payload = i;
        }
        printf("Entering reordering function\n");
        S = reorderRelation(s,&hash1);
        printf("Exited reordering function\n");

        printf("Starting indexing\n");
        indexArray * indexA = indexing(R, hash1, 101);

        printf("Starting searching\n");
        headResult * list = search(*indexA, S, 101);
        printf("Exiting searching Nodes = %d | ResultsPerNode = %d\n", list->numbOfNodes, list->firstNode->size);
        //for(int j = 0; j < list->firstNode->size; j++) {
        //        printf("Row1 = %d | Row2 = %d\n", list->firstNode->tuples[j].rowR, list->firstNode->tuples[j].rowS);
        //}

        if(checkResults(list) == 0) {
                printf("Passed Test\n");
        }
        else {
                printf("Failed Test\n");
        }


        freeResultList(list);
        freeIndexArray(indexA);
        
        free(R->rel->tuples);
	free(S->rel->tuples);
        free(R->rel);
        free(S->rel);
        free(R);
        free(S);


        return 0;
}
