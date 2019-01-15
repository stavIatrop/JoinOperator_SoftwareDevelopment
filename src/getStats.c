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

            if ( TestBit(statsStruct->distinctArray, (col[i] - statsStruct->minI) % N) == 0) {     //check if it is the first occurence of col[i] value and increment distinctVals
                statsStruct->distinctVals++;
            }
 
            SetBit(statsStruct->distinctArray, (col[i] - statsStruct->minI) % N );             //set ((col[i] - statsStruct->minI) % N)-th bit to True
            
        }
        
    } else {

        myint_t size = (myint_t) ceil( (double) (statsStruct->maxU - statsStruct->minI + 1)  / (double)(sizeof(myint_t) * 8) );
        statsStruct->distinctArray = (myint_t *) malloc(size * sizeof(myint_t));
        

        for( i = 0; i < size; i ++)
            statsStruct->distinctArray[i] = 0;
        

        for( i = 0; i < rows; i++) {

            if ( TestBit(statsStruct->distinctArray, col[i] - statsStruct->minI) == 0) {     //check if it is the first occurence of col[i] value and increment distinctVals
                
                statsStruct->distinctVals++;
            }
 
            SetBit(statsStruct->distinctArray, col[i] - statsStruct->minI );             //set (col[i] - statsStruct->minI)-th bit to True
            
        }
    }

    return;
}

void printStats(query * newQuery, relationsheepArray relArray) {

    for(int i = 0; i < newQuery->numOfRels; i++) {

        myint_t cols = relArray.rels[newQuery->rels[i]].cols;
        for(int j = 0; j < cols; j++) {

            fprintf(stderr,"Rel%d Col%d\n", i, j);
            fprintf(stderr,"Min:%ld Max:%ld DistVals:%ld NumElem:%ld\n", relArray.rels[newQuery->rels[i]].statsArray[j].minI, relArray.rels[newQuery->rels[i]].statsArray[j].maxU, relArray.rels[newQuery->rels[i]].statsArray[j].distinctVals, relArray.rels[newQuery->rels[i]].statsArray[j].numElements);
            fprintf(stderr,"Min:%ld Max:%ld DistVals:%ld NumElem:%ld\n", newQuery->queryStats[i][j].minI, newQuery->queryStats[i][j].maxU, newQuery->queryStats[i][j].distinctVals, newQuery->queryStats[i][j].numElements);

        }
    }

}

void executeFilterPredicates(query * newQuery, relationsheepArray relArray ) {

    if (newQuery->numOfFilters != 0 ) {

        for(myint_t i = 0; i < newQuery->numOfFilters; i++) {

            myint_t  numCol = newQuery->filters[i].participant.numCol;
            myint_t indexRel = newQuery->filters[i].participant.rel;                        //rel that participates in particular query

            myint_t originalRel = newQuery->rels[newQuery->filters[i].participant.rel];     //original rel stored in relArray

            myint_t cols = relArray.rels[ originalRel ].cols;
            myint_t fA =  newQuery->queryStats[indexRel][numCol].numElements;  //store fA

            if (newQuery->filters[i].op == 1) {         //less operator
                
                myint_t k2 = newQuery->filters[i].value - 1;

                if (k2 < newQuery->queryStats[indexRel][numCol].maxU) {

                    if( k2 < newQuery->queryStats[indexRel][numCol].minI) {

                        newQuery->queryStats[indexRel][numCol].minI = 0;
                        newQuery->queryStats[indexRel][numCol].maxU = 0;
                        newQuery->queryStats[indexRel][numCol].numElements = 0;
                        newQuery->queryStats[indexRel][numCol].distinctVals = 0;

                    } else if (k2 == newQuery->queryStats[indexRel][numCol].minI) {

                        if(  newQuery->queryStats[indexRel][numCol].distinctVals == 0) {

                            newQuery->queryStats[indexRel][numCol].maxU = 0;
                            newQuery->queryStats[indexRel][numCol].numElements = 0;
                            newQuery->queryStats[indexRel][numCol].minI = 0;
                        } else {

                            newQuery->queryStats[indexRel][numCol].maxU = k2;
                            newQuery->queryStats[indexRel][numCol].numElements = (myint_t) ceil(((double)(newQuery->queryStats[indexRel][numCol].numElements) / (double)(newQuery->queryStats[indexRel][numCol].distinctVals)));
                            newQuery->queryStats[indexRel][numCol].distinctVals = 1;
                        }
                        
                    } else {

                        myint_t prevMax = newQuery->queryStats[indexRel][numCol].maxU;
                        myint_t min = newQuery->queryStats[indexRel][numCol].minI;
                        
                        newQuery->queryStats[indexRel][numCol].maxU = k2;
                        newQuery->queryStats[indexRel][numCol].distinctVals = (myint_t) ceil((((double)(k2-min)/(double)(prevMax-min)) *  newQuery->queryStats[indexRel][numCol].distinctVals));
                        newQuery->queryStats[indexRel][numCol].numElements = (myint_t) ceil((((double)(k2-min)/(double)(prevMax-min)) *  newQuery->queryStats[indexRel][numCol].numElements));
                        
                    }
                }

            }else if(newQuery->filters[i].op == 2) {    //greater operator

                myint_t k1 = newQuery->filters[i].value + 1;
                if ( k1 > newQuery->queryStats[indexRel][numCol].minI ) {

                    if (k1 > newQuery->queryStats[indexRel][numCol].maxU) {
                        
                        newQuery->queryStats[indexRel][numCol].minI = 0;
                        newQuery->queryStats[indexRel][numCol].maxU = 0;
                        newQuery->queryStats[indexRel][numCol].numElements = 0;
                        newQuery->queryStats[indexRel][numCol].distinctVals = 0;

                    }else if( k1 == newQuery->queryStats[indexRel][numCol].maxU){
                        
                        if(  newQuery->queryStats[indexRel][numCol].distinctVals == 0) {

                            newQuery->queryStats[indexRel][numCol].maxU = 0;
                            newQuery->queryStats[indexRel][numCol].numElements = 0;
                            newQuery->queryStats[indexRel][numCol].minI = 0;
                        } else {
                            newQuery->queryStats[indexRel][numCol].minI = k1;
                            newQuery->queryStats[indexRel][numCol].numElements = (myint_t) ceil(((double)(newQuery->queryStats[indexRel][numCol].numElements) / (double)(newQuery->queryStats[indexRel][numCol].distinctVals)));
                            newQuery->queryStats[indexRel][numCol].distinctVals = 1;
                        }
                        

                    } else {

                        myint_t prevMin = newQuery->queryStats[indexRel][numCol].minI;
                        newQuery->queryStats[indexRel][numCol].minI = k1;
                        myint_t max = newQuery->queryStats[indexRel][numCol].maxU;
                        newQuery->queryStats[indexRel][numCol].distinctVals = (myint_t) ceil((((double)(max-k1)/(double)(max-prevMin)) *  newQuery->queryStats[indexRel][numCol].distinctVals));
                        newQuery->queryStats[indexRel][numCol].numElements = (myint_t) ceil((((double)(max-k1)/(double)(max-prevMin)) *  newQuery->queryStats[indexRel][numCol].numElements));
                        
                    }                   

                }
                

            }else if(newQuery->filters[i].op == 3) {       //equal operator

                
                newQuery->queryStats[indexRel][numCol].minI = newQuery->filters[i].value;
                newQuery->queryStats[indexRel][numCol].maxU = newQuery->filters[i].value;

                //check if value exists in distinct values bit vector
                if( TestBit(newQuery->queryStats[indexRel][numCol].distinctArray, newQuery->queryStats[indexRel][numCol].maxU - newQuery->filters[i].value ) == 1) {

                    if(newQuery->queryStats[indexRel][numCol].distinctVals == 0) {

                        newQuery->queryStats[indexRel][numCol].numElements = 0;
                    } else {
                        
                        newQuery->queryStats[indexRel][numCol].numElements = ceil(newQuery->queryStats[indexRel][numCol].numElements / newQuery->queryStats[indexRel][numCol].distinctVals);
                        newQuery->queryStats[indexRel][numCol].distinctVals = 1;
                    }
                    
                    
                } else {

                    newQuery->queryStats[indexRel][numCol].distinctVals = 0;
                    newQuery->queryStats[indexRel][numCol].numElements = 0;
                }

            }

            for( int c = 0; c < cols; c++) {

                    if (c != numCol) {
                        
                        myint_t fANew = newQuery->queryStats[indexRel][numCol].numElements;
                        myint_t fC = newQuery->queryStats[indexRel][c].numElements;
                        myint_t dC = newQuery->queryStats[indexRel][c].distinctVals;
                        if( fA == 0 || dC == 0 ) {

                            newQuery->queryStats[indexRel][c].numElements = 0;
                            newQuery->queryStats[indexRel][c].minI = 0;
                            newQuery->queryStats[indexRel][c].distinctVals =  0;
                            newQuery->queryStats[indexRel][c].maxU = 0;

                        } else {

                            newQuery->queryStats[indexRel][c].numElements = newQuery->queryStats[indexRel][numCol].numElements;
                        
                            newQuery->queryStats[indexRel][c].distinctVals =  (myint_t) ceil((dC * (1 - pow((double) (1 - ((double) fANew/(double) fA)), (double)fC/ (double)dC ))));
                        }
                        
                    }
            }

        }
    }
    if (newQuery->numOfJoins != 0  ) {

        myint_t p = 0;      //index for updating priorities table

        for(myint_t i = 0; i < newQuery->numOfJoins; i++) {

            myint_t rel1 = newQuery->joins[i].participant1.rel;
            myint_t rel2 = newQuery->joins[i].participant2.rel;
            myint_t col1 = newQuery->joins[i].participant1.numCol;
            myint_t col2 = newQuery->joins[i].participant2.numCol;

            
            if( rel1 == rel2 && col1 != col2) {     //second type of filter predicate

                newQuery->priorities[p] = i;           //update priorities table, rest of indexes are initialized to -1
                p++;
                
                myint_t cols = relArray.rels[newQuery->rels[rel1]].cols;
                
                if( newQuery->queryStats[rel1][col1].minI > newQuery->queryStats[rel2][col2].minI) {

                    newQuery->queryStats[rel2][col2].minI = newQuery->queryStats[rel1][col1].minI;
    
                }else {

                    newQuery->queryStats[rel1][col1].minI = newQuery->queryStats[rel2][col2].minI;

                }
                if( newQuery->queryStats[rel1][col1].maxU < newQuery->queryStats[rel2][col2].maxU) {

                    newQuery->queryStats[rel2][col2].maxU = newQuery->queryStats[rel1][col1].maxU;

                } else {

                    newQuery->queryStats[rel1][col1].maxU = newQuery->queryStats[rel2][col2].maxU;
                }

                myint_t lANew = newQuery->queryStats[rel1][col1].minI;
                myint_t uANew = newQuery->queryStats[rel1][col1].maxU;
                myint_t n = uANew - lANew + 1;
                myint_t fA = newQuery->queryStats[rel1][col1].numElements;
                newQuery->queryStats[rel1][col1].numElements = (myint_t) ceil(((double)fA/(double)n));
                newQuery->queryStats[rel2][col2].numElements = newQuery->queryStats[rel1][col1].numElements;

                myint_t fANew = newQuery->queryStats[rel1][col1].numElements;
                myint_t dA = newQuery->queryStats[rel1][col1].distinctVals;

                if( fA == 0 || dA == 0) {

                    newQuery->queryStats[rel1][col1].distinctVals = 0;
                    newQuery->queryStats[rel1][col1].numElements  = 0;
                    newQuery->queryStats[rel1][col1].maxU = 0;
                    newQuery->queryStats[rel1][col1].minI = 0;


                } else {

                    newQuery->queryStats[rel1][col1].distinctVals = (myint_t) ceil((dA * (1 - pow((double) (1 - ((double) fANew/(double) fA)), (double)fA/ (double)dA ))));

                }
                

                for( int c = 0; c < cols; c++) {

                    if (c != col1 && c != col2) {

                        myint_t fANew = newQuery->queryStats[rel1][col1].numElements;
                        myint_t fC = newQuery->queryStats[rel1][c].numElements;
                        myint_t dC = newQuery->queryStats[rel1][c].distinctVals;
                        if( fA == 0 || dC == 0 ) {

                            newQuery->queryStats[rel1][c].distinctVals = 0;
                            newQuery->queryStats[rel1][c].numElements  = 0;
                            newQuery->queryStats[rel1][c].maxU = 0;
                            newQuery->queryStats[rel1][c].minI = 0;
                        } else {

                            newQuery->queryStats[rel1][c].numElements = fANew;
                            newQuery->queryStats[rel1][c].distinctVals = (myint_t) ceil((dC * (1 - pow((double) (1 - ((double) fANew/(double) fA)), (double)fC/ (double)dC ))));
                        }
                        
                    }
                }   

            }
        }

    }
    

    return;
}