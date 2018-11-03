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

//headResult * radixHashJoin(table * rTable, table * sTable, int32_t colR, int32_t colS);

int main(int argc, char *argv[])
{
        //Reading Arguments
        arguments * args = readArguments(argc, argv);
        if(args == NULL) {
                perror("Wrong arguments");
                return -1;
        }

        //Reading Input
        printf(">>> Starting Reading...");
        clock_t begin = clock();
        table * r = readTable(args->rPath, args->type);
        if(r == NULL) {
                return -1;
        }
        table * s = readTable(args->sPath, args->type);
        if(s == NULL) {
                return -1;
        }
        clock_t end = clock();
        printf("Completed in  %f seconds.\n", (double)(end - begin) / CLOCKS_PER_SEC);


        headResult * results = radixHashJoin(r, s, args->colR, args->colS);

        printf(">>> Starting Writing...");
        begin = clock();
        writeList(results, args->outPath);
        end = clock();
        printf("Completed in  %f seconds.\n", (double)(end - begin) / CLOCKS_PER_SEC);

        freeResultList(results);
        freeTable(r);
        freeTable(s);
        free(args);


        return 0;

        //printTable(r);
        //printTable(s);

        //relation * rRel =extractRelation(r, args->colR);
        //relation * sRel =extractRelation(s, args->colS);        
        // uint32_t size = 100000000;
        // relation *r, *s;
	// r = malloc(sizeof(relation));
        // s = malloc(sizeof(relation));
	// r->tuples = malloc(size * sizeof(tuple));
	// r->size=size;
        // s->tuples = malloc(100000000 * sizeof(tuple));
	// s->size=100000000;
	// uint32_t random;

	// srand(time(NULL));
        // for (uint32_t i=0; i<size; i++)
        // {
        //         random = rand();
        //         r->tuples[i].key = random;
        //         r->tuples[i].payload = 837376;
        //         //r->tuples[i].key = 5;
        //         //r->tuples[i].payload = 837376;
        // }

        // for (uint32_t i=0; i<100000000; i++)
        // {
        //         random = rand();
        //         s->tuples[i].key = random;
        //         s->tuples[i].payload = 837376;
        //         //s->tuples[i].key = i;
        //         //s->tuples[i].payload = i;
        // }
        // //Reordering

        // uint32_t h1 = FIRST_REORDERED;
        // reorderedR * RoR = reorderRelation(r, &h1);
        // reorderedR * RoS = reorderRelation(s, &h1);
        // //printf("...H1 = %d | PSUM = %d\n", h1, RoR->pSumArr.psumSize);
        
        // printf("Starting Indexing\n");
        // //Indexing
        // indexArray * indArr = indexing(RoR, h1);

        // //for(int i = 0; i < indArr->size; i++) {
        // //        printf("H2 = %u\n", indArr->indexes[i].hash2);
        // //}
        // //Searching
        // printf("Start Searching\n");
        // headResult * results = search(indArr, RoS);

        //Writing Output

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

}
