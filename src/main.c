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
        uint32_t h1 = 0;
        reorderedR * RoR = reorderRelation(rRel, &h1);
        reorderedR * RoS = reorderRelation(sRel, &h1);

        //Indexing
        indexArray * indArr = indexing(RoR, h1, 32 - h1);
        printf("aaaa\n");
        fflush(stdout);
        //Searching
        headResult * results = search(indArr, RoS);

        //Writing Output
        writeList(results, args->outPath);

        //printRelation(rRel);
        //printRelation(sRel);

        freeTable(r);
        freeTable(s);
        free(args);

        /*headResult * list = initialiseResultHead();

        
        for(int i = 0; i < 10000000; i++) {
                rowTuple temp;
                temp.rowR = (int32_t) i;
                temp.rowS = (uint32_t) i;
                pushResult(list, &temp);
        }

        if(writeList(list, args->outPath) != 0) {
                return -1;
        }*/

        return 0;
}
