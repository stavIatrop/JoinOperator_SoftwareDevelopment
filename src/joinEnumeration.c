#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashTreeManip.h"
#include "combinations.h"
#include "graph.h"

int existsInComb(char * comb, myint_t r) {

    for(int i = 0; i < strlen(comb); i++) {

        char c = comb[i];
        if ( (c - '0') == r)
            return 1;

    }
    return 0;
}

int areConnected(char * comb, myint_t r, graph * joinGraph, myint_t * col1, myint_t * col2) {

    for(int i = 0; i < strlen(comb); i++) {

        char c = comb[i];
        if ( Connected( joinGraph, (c - '0'), r, col1, col2))
            return c - '0';
        if( Connected( joinGraph, r, (c - '0'), col2, col1))
            return c - '0';

    }

    return -1;
}


HTNode * CreateJoinTree(HTNode * HT1, HTNode * HT2 ,char * newComb, myint_t connection, myint_t col1, myint_t col2) {

    // HTNode * newHTNode = (HTNode *) malloc(sizeof(HTNode));
    // newHTNode->comb = (char *) malloc((strlen(newComb) + 1)* sizeof(char));
    // memset(newHTNode->comb, '\0', strlen(newComb) + 1);
    // strcpy(newHTNode->comb, newComb);
    // strcat(newHTNode->comb, "\0");
    // newHTNode->numRels = strlen(newComb);
    // newHTNode->htstats = (HTStats *) malloc((newHTNode->numRels) * sizeof(HTStats));

    // myint_t partA;
    // for(int i = 0; i < strlen(newComb) - 1; i++) {

    //     if(connection == newHTNode->htstats[i].rel) {

    //         partA = i;
    //         break;
    //     }
            
    // }

    // myint_t dA = HT1->htstats[partA].relStats[col1].distinctVals;       //store dA in order to update other columns
    // myint_t dB = HT2->htstats[0].relStats[col2].distinctVals;          //store dB in order to update other columns





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
                myint_t connection = areConnected(combs[s], r , joinGraph, &col1, &col2);
                if( connection == -1) {      //if !connected
                    continue;
                }

                myint_t hashIndex1 = combToIndex(combs[s]);
            
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
                
                


                HTNode * newHTNode = CreateJoinTree(hashTree[hashIndex1], hashTree[hashIndex2], newComb, connection, col1, col2 );

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