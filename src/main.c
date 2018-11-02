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
#include "inputInterface.h"

int main(int argc, char *argv[])
{
        //Reading Arguments
        arguments * args = readArguments(argc, argv);
        if(args == NULL) {
                perror("Wrong arguments");
                return -1;
        }

        //Reading Input
        /*table * r = readTable(args->rPath, args->type);
        if(r == NULL) {
                return -1;
        }
        table * s = readTable(args->sPath, args->type);
        if(s == NULL) {
                return -1;
        }*/

        //printTable(r);
        //printTable(s);

        //relation * rRel =extractRelation(r, args->colR);
        //relation * sRel =extractRelation(s, args->colS);        
        uint32_t size = 10000000;
        relation *r, *s;
	r = malloc(sizeof(relation));
        s = malloc(sizeof(relation));
	r->tuples = malloc(size * sizeof(tuple));
	r->size=size;
        s->tuples = malloc(10000000 * sizeof(tuple));
	s->size=10000000;
	uint32_t random;

	srand(time(NULL));
        for (uint32_t i=0; i<size; i++)
        {
                r->tuples[i].key = 5;
                r->tuples[i].payload = 837376;
        }

        for (uint32_t i=0; i<10000000; i++)
        {
                random = rand();
                //s->tuples[i].key = random;
                //s->tuples[i].payload = 837376;
                s->tuples[i].key = i;
                s->tuples[i].payload = i;
        }
        //Reordering

        uint32_t h1 = FIRST_REORDERED;
        reorderedR * RoR = reorderRelation(r, &h1);
        printf("...H1 = %d | PSUM = %d\n", h1, RoR->pSumArr.psumSize);
        reorderedR * RoS = reorderRelation(s, &h1);
        printf("Starting Indexing\n");
        //Indexing
        indexArray * indArr = indexing(RoR, h1, 11);

        //for(int i = 0; i < indArr->size; i++) {
        //        printf("H2 = %u\n", indArr->indexes[i].hash2);
        //}
        //Searching
        printf("Start Searching\n");
        headResult * results = search(indArr, RoS);

        //Writing Output
        writeList(results, args->outPath);

        /*free(r->tuples);
        free(s->tuples);
        free(RoR->pSumArr.psum);
        free(RoR->rel);
        free(RoR);
        free(RoS->pSumArr.psum);
        free(RoS->rel);
        free(RoS);*/
        
        //freeIndexArray(indArr);
        //freeResultList(results);
        //freeTable(r);
        //freeTable(s);
        free(args);

        return 0;
}
