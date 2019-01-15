#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hashTreeManip.h"
#include "combinations.h"
#include "graph.h"
#include "pipeI_O.h"

int existsInComb(char * comb, myint_t r) {

    for(int i = 0; i < strlen(comb); i++) {

        char c = comb[i];
        if ( (c - '0') == r)
            return 1;

    }
    return 0;
}

int areConnected(char * comb, myint_t r, graph * joinGraph, myint_t * col1, myint_t * col2, myint_t * joinId) {

    for(int i = 0; i < strlen(comb); i++) {

        char c = comb[i];
        if ( Connected( joinGraph, (c - '0'), r, col1, col2, joinId))
            return c - '0';
        if( Connected( joinGraph, r, (c - '0'), col2, col1, joinId))
            return c - '0';

    }

    return -1;
}


void Filter(HTStats * statistics, myint_t k1, myint_t k2, myint_t filterCol) {

    myint_t lA = statistics->relStats[filterCol].minI;
    myint_t uA = statistics->relStats[filterCol].maxU;
    myint_t fA = statistics->relStats[filterCol].numElements;
    myint_t dA = statistics->relStats[filterCol].distinctVals;

    statistics->relStats[filterCol].minI = k1;
    statistics->relStats[filterCol].maxU = k2;
    statistics->relStats[filterCol].distinctVals = ((double)(k2 - k1) / (double) (uA - lA)) * dA;
    statistics->relStats[filterCol].numElements = ((double)(k2 - k1) / (double) (uA - lA)) * fA;
    
    myint_t fANew = statistics->relStats[filterCol].numElements;

    for( int c = 0; c < statistics->cols; c++) {
        
        if(c == filterCol) {
            continue;
        }
        myint_t fC = statistics->relStats[c].numElements;
        myint_t dC = statistics->relStats[c].distinctVals;

        statistics->relStats[c].distinctVals = 1 + (dC * (1 - pow((double) (1 - ((double) fANew/(double) fA)), (double)fC/ (double)dC )));
        statistics->relStats[c].numElements = fANew;
   }
   return;
}

void Join( HTStats * statA, HTStats * statB, myint_t colA, myint_t colB) {

    myint_t min = statA->relStats[colA].minI;
    myint_t max = statA->relStats[colA].maxU;

    myint_t n = max - min + 1;

    myint_t fA = statA->relStats[colA].numElements;
    myint_t fB = statB->relStats[colB].numElements;

    myint_t dA = statA->relStats[colA].distinctVals;
    myint_t dB = statB->relStats[colB].distinctVals;

    statA->relStats[colA].numElements = (double)(fA * fB) / (double) n;
    statB->relStats[colB].numElements = statA->relStats[colA].numElements;
    statA->relStats[colA].distinctVals = (double)(dA * dB) / (double) n;
    statB->relStats[colB].distinctVals = statA->relStats[colA].distinctVals;

    myint_t fANew = statA->relStats[colA].numElements;
    myint_t dANew = statA->relStats[colA].distinctVals;
    myint_t dBNew = statA->relStats[colB].distinctVals;

    for(int c = 0; c < statA->cols; c++) {

        if( c == colA) {
            continue;
        }
        
        myint_t dC = statA->relStats[c].distinctVals;
        myint_t fC = statA->relStats[c].numElements;

        statA->relStats[c].distinctVals = 1 + (dC * (1 - pow((double) (1 - ((double) dANew/(double) dA)), (double)fC/ (double)dC )));
        statA->relStats[c].numElements = fANew;
    }

    for(int c = 0; c < statB->cols; c++) {

        if( c == colB) {
            continue;
        }
        
        myint_t dC = statB->relStats[c].distinctVals;
        myint_t fC = statB->relStats[c].numElements;

        statB->relStats[c].distinctVals = 1 + (dC * (1 - pow((double) (1 - ((double) dBNew/(double) dB)), (double)fC/ (double)dC )));
        statB->relStats[c].numElements = fANew;
    }

    return;


}


HTNode * CreateJoinTree(HTNode * HT1, HTNode * HT2 ,char * newComb, myint_t connection, myint_t col1, myint_t col2, myint_t joinId) {

    
    HTNode * newHTNode = (HTNode *) malloc(sizeof(HTNode));
    newHTNode->comb = (char *) malloc((strlen(newComb) + 1)* sizeof(char));
    memset(newHTNode->comb, '\0', strlen(newComb) + 1);
    strcpy(newHTNode->comb, newComb);
    strcat(newHTNode->comb, "\0");
    newHTNode->numRels = strlen(newComb);
    newHTNode->htstats = (HTStats *) malloc((newHTNode->numRels) * sizeof(HTStats));

    if(HT1->joinSeq == NULL) {

        newHTNode->joinSeq = (char *) malloc((numDigits(joinId) + 1) * sizeof(char));
        memset(newHTNode->joinSeq, '\0', numDigits(joinId) + 1);
        sprintf(newHTNode->joinSeq, "%ld", joinId);
        strcat(newHTNode->joinSeq, "\0");

    } else {

        newHTNode->joinSeq = (char *) malloc((numDigits(joinId) + strlen(HT1->joinSeq) + 1) * sizeof(char));
        memset(newHTNode->joinSeq, '\0', numDigits(joinId) + strlen(HT1->joinSeq) + 1);
        strcpy(newHTNode->joinSeq, HT1->joinSeq);
        char * str = (char *) malloc( (numDigits(joinId) + 1) * sizeof(char));
        memset(str, '\0',numDigits(joinId) + 1 );
        sprintf(str, "%ld", joinId);
        strcat(newHTNode->joinSeq, str);
        strcat(newHTNode->joinSeq, "\0");
        free(str);

    }
    myint_t partA;
    
    for(int i = 0; i < newHTNode->numRels - 1; i++) {           //copy statistics of previous combination to new hashTree node
        
        newHTNode->htstats[i].cols = HT1->htstats[i].cols;
        newHTNode->htstats[i].rel = HT1->htstats[i].rel;
        
        if( HT1->htstats[i].rel == connection) {
            
            partA = i;
        }
        newHTNode->htstats[i].relStats = (stats *) malloc(newHTNode->htstats[i].cols * sizeof(stats));
        for( int j = 0; j < newHTNode->htstats[i].cols; j++){

            newHTNode->htstats[i].relStats[j].minI = HT1->htstats[i].relStats[j].minI;    
            newHTNode->htstats[i].relStats[j].maxU = HT1->htstats[i].relStats[j].maxU;    
            newHTNode->htstats[i].relStats[j].numElements = HT1->htstats[i].relStats[j].numElements;    
            newHTNode->htstats[i].relStats[j].distinctVals = HT1->htstats[i].relStats[j].distinctVals;    
        }
    }

    myint_t partB = newHTNode->numRels - 1;

    newHTNode->htstats[partB].cols = HT2->htstats[0].cols;
    newHTNode->htstats[partB].rel = HT2->htstats[0].rel;
    newHTNode->htstats[partB].relStats = (stats *) malloc(newHTNode->htstats[partB].cols * sizeof(stats));
    for( int j = 0; j < newHTNode->htstats[partB].cols; j++){
        
        newHTNode->htstats[partB].relStats[j].minI = HT2->htstats[0].relStats[j].minI;    
        newHTNode->htstats[partB].relStats[j].maxU = HT2->htstats[0].relStats[j].maxU;    
        newHTNode->htstats[partB].relStats[j].numElements = HT2->htstats[0].relStats[j].numElements;    
        newHTNode->htstats[partB].relStats[j].distinctVals = HT2->htstats[0].relStats[j].distinctVals;    
    }
    
    myint_t newMin, newMax;

    if ( newHTNode->htstats[partA].relStats[col1].minI > newHTNode->htstats[partB].relStats[col2].minI) {

        newMin = newHTNode->htstats[partA].relStats[col1].minI;
    } else {

        newMin = newHTNode->htstats[partB].relStats[col2].minI;
    }


    if( newHTNode->htstats[partA].relStats[col1].maxU < newHTNode->htstats[partB].relStats[col2].maxU ) {

        newMax = newHTNode->htstats[partA].relStats[col1].maxU;
    } else {

        newMax = newHTNode->htstats[partB].relStats[col2].maxU;

    }
    
    myint_t fA = newHTNode->htstats[partA].relStats[col1].numElements; 

    Filter(&(newHTNode->htstats[partA]), newMin, newMax, col1);

    myint_t fANew = newHTNode->htstats[partA].relStats[col1].numElements;

    //update rest columns of Intermediate
    for( int r = 0; r < newHTNode->numRels - 1; r++) {
        
        if( r == partA) {
            continue;
        }

        for(int c = 0; c < newHTNode->htstats[r].cols; c++) {

            myint_t fC = newHTNode->htstats[r].relStats[c].numElements;
            myint_t dC = newHTNode->htstats[r].relStats[c].distinctVals;

            newHTNode->htstats[r].relStats[c].distinctVals = 1 + (dC * (1 - pow((double) (1 - ((double) fANew/(double) fA)), (double)fC/ (double)dC )));
            newHTNode->htstats[r].relStats[c].numElements = fANew;

        }

    }
    Filter(&(newHTNode->htstats[partB]), newMin, newMax, col2);

    myint_t dA = newHTNode->htstats[partA].relStats[col1].distinctVals;

    Join(&(newHTNode->htstats[partA]), &(newHTNode->htstats[partB]), col1, col2);

    myint_t dANew = newHTNode->htstats[partA].relStats[col1].distinctVals;;
    fANew = newHTNode->htstats[partA].relStats[col1].numElements;

    for( int r = 0; r < newHTNode->numRels - 1; r++) {

        
        if( r == partA) {
            continue;
        }

        for(int c = 0; c < newHTNode->htstats[r].cols; c++) {

            myint_t fC = newHTNode->htstats[r].relStats[c].numElements;
            myint_t dC = newHTNode->htstats[r].relStats[c].distinctVals;

            newHTNode->htstats[r].relStats[c].distinctVals = 1 + (dC * (1 - pow((double) (1 - ((double) dANew/(double) dA)), (double)fC/ (double)dC )));
            newHTNode->htstats[r].relStats[c].numElements = fANew;

        }

    }

    newHTNode->cost = HT1->cost + fANew;

    return newHTNode;
}

void joinEnumeration(HTNode ** hashTree, query *newQuery, relationsheepArray relArray, graph * joinGraph ) {

    myint_t numRels = newQuery->numOfRels;
    for(int i = 1; i < numRels; i++) {

        myint_t numCombs = Factorial(numRels) / (Factorial(i) * Factorial(numRels-i));
        char ** combs = (char **) malloc( numCombs * sizeof(char *) );

        getCombination(numRels, i, combs);      
        for(myint_t s = 0; s < numCombs; s++) {         //for all  S

            for(myint_t r = 0; r < numRels; r++) {       //for all Rj

                if(existsInComb(combs[s], r)) {
                    continue;
                }

                myint_t col1, col2;
                myint_t joinId;
                myint_t connection = areConnected(combs[s], r , joinGraph, &col1, &col2, &joinId);
                if( connection == -1) {      //if !connected
                    continue;
                }

                myint_t hashIndex1 = combToIndex(combs[s]);
                if (hashTree[hashIndex1] == NULL) {
                    continue;
                }
                myint_t hashIndex2 = 0;
                hashIndex2 = hashIndex2 | (1 << r);
                char * newComb = (char *) malloc((strlen(combs[s]) + 2) * sizeof(char));
                memset(newComb, '\0', strlen(combs[s]) + 2);
                strcpy(newComb, combs[s]);
                char * str = (char *) malloc(2 * sizeof(char));
                memset(str, '\0', 2);
                sprintf(str, "%ld" ,r);
                strcat(newComb, str);
                free(str);
                strcat(newComb, "\0");
                
                
                HTNode * newHTNode = CreateJoinTree(hashTree[hashIndex1], hashTree[hashIndex2], newComb, connection, col1, col2, joinId );
                
                myint_t indexHT = combToIndex(newComb);
                if ( hashTree[indexHT] == NULL || (hashTree[indexHT]->cost > newHTNode->cost)) {

                    if(hashTree[indexHT] != NULL) {

                        for(int j = 0; j < hashTree[indexHT]->numRels; j++) {

                            free(hashTree[indexHT]->htstats[j].relStats);
                        }
                        free(hashTree[indexHT]->htstats);

                        free(hashTree[indexHT]->comb);
                        if(hashTree[indexHT]->joinSeq != NULL)
                            free(hashTree[indexHT]->joinSeq);

                        free(hashTree[indexHT]);
                    }
                    hashTree[indexHT] = newHTNode;

                } else {

                    for(int j = 0; j < newHTNode->numRels; j++) {

                        free(newHTNode->htstats[j].relStats);
                    }

                    free(newHTNode->htstats);
                    free(newHTNode->comb);
                    if(newHTNode->joinSeq != NULL)
                        free(newHTNode->joinSeq);
                    
                    free(newHTNode);
                    
                }
                
                free(newComb);
            }
        }

        for(int j = 0; j < numCombs; j++) {

            free(combs[j]);
        }
        
        free(combs);
    }
    return;
}

// void SetPriorities(char * sequence, query * newQuery){

//     myint_t i = 0;
//     while( newQuery->priorities[i] != -1) {
//         i++;
//     }

//     for(int j = 0; j < strlen(sequence); j++) {

//         char c = sequence[j];
//         myint_t joinId = charToInteger(c);
//         newQuery->priorities[i] = joinId;
//         i++;
//     }
//     myint_t alreadyPrioritized = i;

//     //it will change(filter joins)
//     if( i < newQuery->numOfJoins) {

//         for(int j = i; j < newQuery->numOfJoins; j++) {

//             for(myint_t k = 0; k < newQuery->numOfJoins; k++) {  //iterate through joins in order to set priority to those who
//                if(existsInComb(sequence, k)) {                     //are done between rels that have already participate to previous joins
//                 if(existsInComb(sequence, k)) {
//                     continue;
//                 }

//                 int p;
//                 for(p = 0; p < alreadyPrioritized - 1; p++) {
                    
//                     myint_t partA1 = newQuery->joins[newQuery->priorities[p]].participant1.rel;
//                     myint_t partB1 = newQuery->joins[p].participant1.rel;
//                     myint_t partA2 = newQuery->joins[newQuery->priorities[p]].participant2.rel;
//                     myint_t partB2 = newQuery->joins[p].participant2.rel;
//                     if( partA1 == partB1 && partA2 == partB2 ) {
//                         break;
//                     }else if( partA1 == partB2 && partA2 == partB1) {
//                         break;
//                     }
//                 }
//                 if(p != alreadyPrioritized - 1) {       //break condition
//                     int a;
//                     alreadyPrioritized++;
                    
//                     for( a = j; a > p + 1; a--) {

//                         newQuery->priorities[a] = newQuery->priorities[a - 1];
//                     }
//                     newQuery->priorities[a] = k;
//                 } else {
//                     newQuery->priorities[j] = k;
//                 }
                
//             }
//         }
//     }
//     return;
// }

void SetPriorities(char * sequence, query * newQuery){

    myint_t i = 0;
    while( newQuery->priorities[i] != -1) {
        i++;
    }

    for(int j = 0; j < strlen(sequence); j++) {

        char c = sequence[j];
        myint_t joinId = charToInteger(c);
        newQuery->priorities[i] = joinId;
        i++;
    }
    myint_t alreadyPrioritized = i;

    //it will change(filter joins)
    if( i < newQuery->numOfJoins) {

        for(int j = i; j < newQuery->numOfJoins; j++) {

            for(myint_t k = 0; k < newQuery->numOfJoins; k++) {     //iterate through joins in order to set priority to those who
                if(existsInComb(sequence, k)) {                     //are done between rels that have already participate to previous joins
                    continue;
                }

                myint_t p, flag;
                flag = 0;
                for(p = 0; p < alreadyPrioritized; p++) {
                    //fprintf(stderr, "already:%ld k:%ld p:%ld\n", alreadyPrioritized, k, p);
                    myint_t partA1 = newQuery->joins[newQuery->priorities[p]].participant1.rel;
                    myint_t partA1col = newQuery->joins[newQuery->priorities[p]].participant1.numCol;
                    myint_t partB1 = newQuery->joins[k].participant1.rel;
                    myint_t partB1col = newQuery->joins[k].participant1.numCol;
                    myint_t partA2 = newQuery->joins[newQuery->priorities[p]].participant2.rel;
                    myint_t partA2col = newQuery->joins[newQuery->priorities[p]].participant2.numCol;
                    myint_t partB2 = newQuery->joins[k].participant2.rel;
                    myint_t partB2col = newQuery->joins[k].participant2.numCol;

                    //fprintf(stderr, "partA1:%ld, partA1col:%ld, partA2:%ld, partA2col:%ld, partB1:%ld, partB1col: %ld,  partB2:%ld, partB2col:%ld\n", partA1,partA1col, partA2, partA2col, partB1, partB1col, partB2, partB2col);
                    if( partA1 == partB1 && partA2 == partB2 ) {
                        if(partA1col == partB1col && partA2col == partB2col) { //if join is identical with another one, skip it
                            flag = 1;
                            
                        }
                        break;
                    }else if( partA1 == partB2 && partA2 == partB1) {
                        if(partA1col == partB2col && partA2col == partB1col) {  //if join is identical with another one, skip it
                            flag = 1;
                            
                        }
                        break;
                    }
                }
                if(flag == 1) {
                    continue;
                }
                if(p != alreadyPrioritized) {       //break condition
                    //fprintf(stderr, "I'm broken...\n");
                    int a;
                    alreadyPrioritized++;
                    
                    for( a = j; a > p + 1; a--) {

                        newQuery->priorities[a] = newQuery->priorities[a - 1];
                    }
                    newQuery->priorities[a] = k;
                }
                else {
                    newQuery->priorities[j] = k;
                }
                
            }
        }
    }
    return;
}