#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "I_O_structs.h"
#include "queryStructs.h"


int main(void) {

    Input * input = InitializeInput();
    FILE * stdoutFile = fopen("stdoutFile", "r+w");

    fprintf(stdoutFile, "start\n");
    //write(STDOUT_FILENO, "Ready\n", strlen("Ready\n")); //may change 

	while(1) {
		char buffer[4096];
		
    	ssize_t bytes = read(STDIN_FILENO, buffer, sizeof(buffer));
		if (bytes < 0) {
      		if (errno == EINTR) continue;
      		perror("read");
     		exit(EXIT_FAILURE);
    	}
    	if (bytes == 0) break;

    	int end = 0, start = 0;

    	for(int i = 0; i < bytes; i++) {

            fprintf(stdoutFile, "%s\n", "Done" );
    		if( buffer[i] == '\n') {
    			start = end;
    			end = i + 1;

    			if(strncmp("Done", buffer + start, i - start) == 0) {
                    fprintf(stdoutFile, "%s\n", "Done" );
    				break;
    			}
                char * filename = (char *) malloc((i - start + 1) * sizeof(char));
                strncpy(filename, buffer + start, i - start);
                AddInputNode(input,filename);
                free(filename);
    		}

    	}
	}

    relationsheepArray relArray =  InitializeRelSheepArray(input->numNodes);
    FillRelArray(&relArray, input);
    FreeInput(input);

    write(STDOUT_FILENO, "Ready\n", strlen("Ready\n")); //may change 

	while(1){
		
	}
	
	return 0;

}