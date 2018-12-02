#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "I_O_structs.h"
#include "queryStructs.h"
#include "pipeI_O.h"
#include "queryManip.h"


int main(void) {

    
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
    
    fprintf(stdoutFile, "Writing Ready...\n");
    fflush(stdoutFile);
    write(STDOUT_FILENO, "Ready\n", strlen("Ready\n")); //Send signal to harness in order to start sending queries 
    
    fprintf(stdoutFile, "Ready\n");
    fflush(stdoutFile);


    while(1) {

        inputStr = readFromPipe("F");
        
        if( strlen(inputStr) == 0) {        //no more query batches
            break;
        }

        int end = 0, start = 0;
        myint_t i, rels = 1, joins = 0, sums = 1, filters = 0;
        myint_t slice = 0;
        for(i = 0; i < strlen(inputStr); i++) {

                // fprintf(stdoutFile, "/.........\n");
                // fflush(stdoutFile);
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
                    else if ( slice == 1)
                        joins++;
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

                    fprintf(stdoutFile, "QueryBEF:%s\n", queryStr);
                    fflush(stdoutFile);
                    query * newQuery = ConstructQuery(stdoutFile, queryStr, rels, joins, sums, filters, relArray);
                   
                    //cheksum * cSum = ValFunction(query, ...);
                    
                    free(newQuery->rels);
                    free(newQuery->filters);
                    free(newQuery->joins);
                   
                    free(newQuery->sums);
                    free(newQuery);                   
                    fprintf(stdoutFile, "QueryAFTER:%s\n", queryStr);
                    fflush(stdoutFile);
                    
                    free(queryStr);
                    
                    rels = 1, joins = 0, sums = 1, filters = 0, slice = 0;
                }
        }
        

    }
    

        
    fprintf(stdoutFile, "%s\n", inputStr);
    fflush(stdoutFile);



	

    

    if( fclose(stdoutFile) ) {
        perror("Failed to close file");
        return -1;
    }
    while(1) {

    }
	
	return 0;

}