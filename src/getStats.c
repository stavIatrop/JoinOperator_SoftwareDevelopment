#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "queryStructs.h"
#include "getStats.h"
#include "bitVector.h"

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


    if ( statsStruct->maxU - statsStruct->minI + 1 > N ) {
        
        myint_t size = (myint_t) ceil( (double)N / (double)(sizeof(myint_t) * 8) );
        statsStruct->distinctArray = (myint_t *) malloc( size * sizeof(myint_t) );

        for( i = 0; i < size; i ++)
            statsStruct->distinctArray[i] = 0;

        for( i = 0; i < rows; i++) {

            if ( TestBit(statsStruct->distinctArray, (statsStruct->maxU - col[i]) % N) == 0) {     //check if it is the first occurence of col[i] value and increment distinctVals
                statsStruct->distinctVals++;
            }
 
            SetBit(statsStruct->distinctArray, (statsStruct->maxU - col[i]) % N );             //set ((statsStruct->maxU - col[i]) % N)-th bit to True
            
        }
        
    } else {

        myint_t size = (myint_t) ceil( (double) (statsStruct->maxU - statsStruct->minI + 1)  / (double)(sizeof(myint_t) * 8) );
        statsStruct->distinctArray = (myint_t *) malloc(size * sizeof(myint_t));
        

        for( i = 0; i < size; i ++)
            statsStruct->distinctArray[i] = 0;
        

        for( i = 0; i < rows; i++) {

            if ( TestBit(statsStruct->distinctArray, statsStruct->maxU - col[i]) == 0) {     //check if it is the first occurence of col[i] value and increment distinctVals
                
                statsStruct->distinctVals++;
            }
 
            SetBit(statsStruct->distinctArray, statsStruct->maxU - col[i] );             //set (statsStruct->maxU - col[i])-th bit to True
            
        }
    }

    return;
}


void PredicateSequence(query * newQuery, relationsheepArray relArray ) {

    if (newQuery->numOfFilters != 0 ) {

        for(myint_t i = 0; i < newQuery->numOfFilters; i++) {

            if (newQuery->filters[i].op == 1) {         //less operator
                


            }else if(newQuery->filters[i].op == 2) {    //greater operator

            }else if(newQuery->filters[i].op == 3) {       //equal operator

                myint_t  numCol = newQuery->filters[i].participant.numCol;
                myint_t indexRel = newQuery->rels[newQuery->filters[i].participant.rel];
                relArray.rels[ indexRel ].queryStats[numCol].minI = newQuery->filters[i].value;
                relArray.rels[ indexRel ].queryStats[numCol].maxU = newQuery->filters[i].value;

                //check if value exists in distinct values bit vector
                if( TestBit(relArray.rels[ indexRel ].queryStats[numCol].distinctArray, relArray.rels[ indexRel ].queryStats[numCol].maxU - newQuery->filters[i].value ) == 1) {

                    relArray.rels[ indexRel ].queryStats[numCol].distinctVals = 1;
                    relArray.rels[ indexRel ].queryStats[numCol].numElements = relArray.rels[ indexRel ].queryStats[numCol].numElements / relArray.rels[ indexRel ].queryStats[numCol].distinctVals;
                } else {

                    relArray.rels[ indexRel ].queryStats[numCol].distinctVals = 0;
                    relArray.rels[ indexRel ].queryStats[numCol].numElements = 0;
                }

                myint_t cols = relArray.rels[ indexRel ].cols;
                for( int c = 0; c < cols; c++) {

                    if (c != numCol) {

                        relArray.rels[ indexRel ].queryStats[c].numElements = relArray.rels[ indexRel ].queryStats[numCol].numElements;
                       // relArray.rels[ indexRel ].queryStats[c].distinctVals = relArray.rels[ indexRel ].queryStats[c].distinctVals * (1 - )
                    }
                }


                

            }

        }
    }

    return;
}