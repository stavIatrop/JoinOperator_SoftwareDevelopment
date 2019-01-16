#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "I_O_structs.h"
#include "queryStructs.h"
#include "pipeI_O.h"
#include "queryManip.h"
#include "indexManip.h"
#include "checksumInterface.h"
#include "jointPerformer.h"
#include "interListInterface.h"
#include "jobScheduler.h"
#include "basicStructs.h"

void printJoins(query * newQuery) {
    fprintf(stderr, "    Printing Joins of Query\n");
    for(myint_t i = 0; i < newQuery->numOfJoins; i++) {
        fprintf(stderr, "        %ld . Rel1: %ld Rows1: %ld Test1: %ld | Rel2: %ld Rows2: %ld Test2: %ld\n",i, newQuery->joins[i].participant1.rel, newQuery->joins[i].participant1.rows, newQuery->joins[i].participant1.col[0], newQuery->joins[i].participant2.rel, newQuery->joins[i].participant2.rows, newQuery->joins[i].participant2.col[0]);
    }
}

void printFilters(query * newQuery) {
    fprintf(stderr, "    Printing Filters of Query\n");
    for(myint_t i = 0; i < newQuery->numOfFilters; i++) {
        fprintf(stderr, "        %ld . Rel: %ld | Rows: %ld | Test: %ld | Op: %d | Value: %ld\n",i, newQuery->filters[i].participant.rel, newQuery->filters[i].participant.rows, newQuery->filters[i].participant.col[0], newQuery->filters[i].op, newQuery->filters[i].value);
    }
}

void printChecksums(query * newQuery) {
    fprintf(stderr, "    Printing Checksums of Query\n");
    for(myint_t i = 0; i < newQuery->numOfSums; i++) {
        fprintf(stderr, "        %ld . Rel: %ld | Rows: %ld | Test: %ld\n",i, newQuery->sums[i].rel, newQuery->sums[i].rows, newQuery->sums[i].col[0]);
    }
}

int main(void) {

    myint_t whichQuery = 0;
    
    FILE * stdoutFile = fopen("stdoutFile", "w");  //checking the outpout

    if(stdoutFile == NULL) {
        perror("Failed to open file\n");
        return -1;
    }
    fprintf(stdoutFile, "start\n");
    fflush(stdoutFile);
    

    Input * input = InitializeInput();

    char * inputStr = readFromPipe("Done");     //function for reading from pipe until "Done" is received

    fprintf(stdoutFile, "%s\n", inputStr);
    fflush(stdoutFile);

    ConstructInput(input, inputStr);            //Construct linked list of input filenames
    
    free(inputStr);


    fprintf(stdoutFile, "Building relArray...\n");
    fflush(stdoutFile);
    
    
    relationsheepArray relArray =  InitializeRelSheepArray(input->numNodes);
    FillRelArray(&relArray, input);
    FreeInput(input);
    
    // fprintf(stdoutFile, "Writing Ready...\n");
    // fflush(stdoutFile);
    // write(STDOUT_FILENO, "Ready\n\0", strlen("Ready\n\0")); //Send signal to harness in order to start sending queries 
    
    // fprintf(stdoutFile, "Ready\n");
    // fflush(stdoutFile);

    initialiseScheduler();
    while(1) {


        inputStr = readFromPipe("F");
        
        if( strlen(inputStr) == 0) {        //no more query batches
        	free(inputStr);
            break;
        }

        myint_t end = 0, start = 0;
        myint_t i, rels = 1, joins = 0, sums = 1, filters = 0;
        myint_t slice = 0;
        for(i = 0; i < strlen(inputStr); i++) {

                if (inputStr[i] == ' ') {
                
                    if ( slice == 0)
                        rels++;
                    else if ( slice == 2)
                        sums++;
                    else {
                        perror("Wrong Input Format0");
                        return -1;
                    }
                
                }else if (inputStr[i] == '|') {
                    slice++;               
                }

                if (inputStr[i] == '=') {

                    if ( slice == 0) {
                        perror("Wrong Input Format1");
                        return -1;
                    }
                    else if ( slice == 1){
                        if(inputStr[(i + 2) ] == '.' || inputStr[(i + 3)] == '.')
                            joins++;
                        else
                            filters++;
                    }
                    else {
                        perror("Wrong Input Format2");
                        return -1;
                    }

                }

                if (inputStr[i] == '>' || inputStr[i] == '<') {

                    if ( slice == 0) {
                        perror("Wrong Input Format3");
                        return -1;
                    }
                    else if ( slice == 1)
                        filters++;
                    else {
                        perror("Wrong Input Format4");
                        return -1;
                    }

                }
                if( inputStr[i] == '\n') {
                    start = end;
                    end = i + 1;

                    char * queryStr = (char *) malloc((i - start + 1) * sizeof(char));
                    memset(queryStr, '\0', (i - start + 1) );
                    strncpy(queryStr, inputStr + start, i - start);
                    strcat(queryStr, "\0");

                    
                    query * newQuery = ConstructQuery(queryStr, rels, joins, sums, filters, relArray);
                    //printJoins(newQuery);


                    headInter * headInt = initialiseHead();

                    //Perform filters
                    for(myint_t whichFilter = 0; whichFilter < newQuery->numOfFilters; whichFilter++) {
                        workerF(&(newQuery->filters[whichFilter]), headInt);
                        fflush(stdout);
                    }

                    //Perform joins
                    for(myint_t whichJoin = 0; whichJoin < newQuery->numOfJoins; whichJoin++) {
                        //fprintf(stderr,"LLL\n");
                        workerJ(&(newQuery->joins[whichJoin]), headInt);
                    }


                    //Perform checksums
                    checksum * cs = performChecksums(newQuery->sums, newQuery->numOfSums, headInt);

                    freeInterList(headInt);

                    //Write checksums
                    writePipe(cs);

                    free(cs->checksums);
                    free(cs);

                    

                    FreeQuery(newQuery);
                    
                    free(queryStr);
                    
                    rels = 1;
                    joins = 0;
                    sums = 1;
                    filters = 0;
                    slice = 0;
                    whichQuery++;
                }
        }

    free(inputStr);
        
    }
    

    if( fclose(stdoutFile) ) {
        perror("Failed to close file");
        return -1;
    }
    FreeRelArray(relArray);
    if (indexWarehouse)
    {
        for (myint_t i=0; i<indexWarehouse->size;i++) freeIndexArray(indexWarehouse->indexes[i]);
        free(indexWarehouse->rel);
        free(indexWarehouse->col);
        free(indexWarehouse->hash1);
        free(indexWarehouse->indexes);
        free(indexWarehouse);
    }
	
    shutdownAndFreeScheduler();

    fprintf(stderr, "Radixes: %d | Avg radix time: %f\n", radixes, totalRadixTime / (double) radixes);
    
	return 0;

}