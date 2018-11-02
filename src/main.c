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
        table * r = readTable(args->rPath, args->type);
        if(r == NULL) {
                return -1;
        }
        table * s = readTable(args->sPath, args->type);
        if(s == NULL) {
                return -1;
        }

        //printTable(r);
        //printTable(s);

        relation * rRel =extractRelation(r, args->colR);
        relation * sRel =extractRelation(s, args->colS);

        //Reordering
        uint32_t h1 = FIRST_REORDERED;
        reorderedR * RoR = reorderRelation(rRel, &h1);
        printf("...H1 = %d | PSUM = %d\n", h1, RoR->pSumArr.psumSize);
        reorderedR * RoS = reorderRelation(sRel, &h1);

        //Indexing
        indexArray * indArr = indexing(RoR, h1, 6);

        //for(int i = 0; i < indArr->size; i++) {
        //        printf("H2 = %u\n", indArr->indexes[i].hash2);
        //}
        //Searching
        headResult * results = search(indArr, RoS);

        //Writing Output
        writeList(results, args->outPath);

        freeTable(r);
        freeTable(s);
        free(args);

        return 0;
}
