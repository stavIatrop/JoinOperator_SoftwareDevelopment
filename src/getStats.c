#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "queryStructs.h"
#include "getStats.h"

void FillStatsArray(myint_t * col, stats * statsStruct, myint_t rows ) {

    statsStruct->minI = col[0];
    statsStruct->maxU = col[0];
    statsStruct->numElements = rows;
    statsStruct->distinctVals = 0;

    myint_t i;
    for( i = 0; i < rows; i++) {

        if (statsStruct->minI > col[i] ) {
            statsStruct->minI = col[i];
        }

        if ( statsStruct->maxU < col[i] ) {
            statsStruct->maxU = col[i];
        }

    }
    bool * distinctArray;
    if ( statsStruct->maxU - statsStruct->minI + 1 > N ) {
        
        distinctArray = (bool *) malloc( N * sizeof(bool));

        for( i = 0; i < N; i ++)
            distinctArray[i] = false;

         for( i = 0; i < rows; i++) {

            if ( distinctArray[ (statsStruct->maxU - col[i]) % N ] == false) {
                statsStruct->distinctVals++;
            }

            distinctArray[ (statsStruct->maxU - col[i]) % N] = true;

        }

    } else {

        distinctArray = (bool *) malloc((statsStruct->maxU - statsStruct->minI + 1) * sizeof(bool));

        for( i = 0; i < statsStruct->maxU - statsStruct->minI + 1; i ++)
            distinctArray[i] = false;
        

        for( i = 0; i < rows; i++) {

            if ( distinctArray[statsStruct->maxU - col[i]] == false) {
                statsStruct->distinctVals++;
            }
 
            distinctArray[statsStruct->maxU - col[i]] = true;

        }
    }

    free(distinctArray);

    return;
}