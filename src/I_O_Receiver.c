#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "I_O_structs.h"
#include "queryStructs.h"
#include "pipeI_O.h"


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