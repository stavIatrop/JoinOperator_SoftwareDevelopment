#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "basicStructs.h"
#include "queryStructs.h"
#include "resultListInterface.h"
#include "checksumInterface.h"

checksum * performChecksums(colRel * sums, int numbOfSums, headInter * headInt) {
    if(headInt->numOfIntermediates != 1) {
        printf("CASE: Multiple Intermediates at the end NOT IMPELEMENTED\n");
    }
    checksum * retChecksum = (checksum *) malloc(sizeof(checksum));

    retChecksum->checksums = (myint_t *) malloc(numbOfSums * sizeof(myint_t));
    retChecksum->numbOfChecksums = numbOfSums;


    for(int whichSum = 0; whichSum < numbOfSums; whichSum++) {
        for(int whichRel = 0; whichRel < headInt->start->data->numOfCols; whichRel++) {
            if(sums[whichSum].rel == headInt->start->data->joinedRels[whichRel]) {
                retChecksum->checksums[whichSum] = calcChecksum(headInt->start->data->rowIds, headInt->start->data->numbOfRows, whichRel, sums[whichSum].col);
                break;
            }
        }
    }

    return retChecksum;
}

myint_t calcChecksum(myint_t ** intValues, myint_t intRows, int32_t intCol,  myint_t * relValues) {
    myint_t sum = 0;

    for(int whichRow = 0; whichRow < intRows; whichRow++) {
        sum += relValues[intValues[intCol][whichRow]];
    }

    return sum;
}
