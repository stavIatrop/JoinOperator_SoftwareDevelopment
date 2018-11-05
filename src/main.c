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

        //Performing radixHashJoin
        headResult * results = radixHashJoin(r, s, args->colR, args->colS);

        printf(">>> Starting Writing...");
        begin = clock();
        
        //Writing Result List on outFile
        writeList(results, args->outPath);
        end = clock();
        printf("Completed in  %f seconds.\n", (double)(end - begin) / CLOCKS_PER_SEC);

        freeResultList(results);
        freeTable(r);
        freeTable(s);
        free(args);


        return 0;

}
