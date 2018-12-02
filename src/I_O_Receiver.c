#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "I_O_structs.h"
#include "queryStructs.h"


int main(void) {

    
    FILE * stdoutFile = fopen("stdoutFile", "w");

    if(stdoutFile == NULL) {
        perror("Failed to open file\n");
        return -1;
    }
    fprintf(stdoutFile, "start\n");
    fflush(stdoutFile);
    // if( fclose(stdoutFile) ) {
    //     perror("Failed to close file");
    //     return -1;
    // }
    Input * input = InitializeInput();

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
        int i;
    	for(i = 0; i < bytes; i++) {

    		if( buffer[i] == '\n') {
    			start = end;
    			end = i + 1;

    			if(strncmp("Done", buffer + start, i - start) == 0) {

                    
                    fprintf(stdoutFile, "Done\n");
                    fflush(stdoutFile);
                    
    				break;
    			}
                char * filename = (char *) malloc((i - start + 1) * sizeof(char));
                strncpy(filename, buffer + start, i - start);

                
                AddInputNode(input,filename);
                
                free(filename);
    		}
    	}
        if( i < bytes) {
            break;
        }
	}
    
    fprintf(stdoutFile, "Building relArray...\n");
    fflush(stdoutFile);
    
    
    relationsheepArray relArray =  InitializeRelSheepArray(input->numNodes);
    FillRelArray(&relArray, input);
    FreeInput(input);
    
    fprintf(stdoutFile, "Writing Ready...\n");
    fflush(stdoutFile);
    write(STDOUT_FILENO, "Ready\n", strlen("Ready\n")); //may change 
    
    fprintf(stdoutFile, "Ready\n");
    fflush(stdoutFile);

    if( fclose(stdoutFile) ) {
        perror("Failed to close file");
        return -1;
    }

	while(1){
		
        
	}
	
	return 0;

}