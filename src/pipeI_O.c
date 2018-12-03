#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "pipeI_O.h"

char * readFromPipe(const char * stopWord) {


	int lenInput = MAX_BUFFER + 1;
    char * inputStr = (char *) malloc(lenInput * sizeof(char));
    memset(inputStr, '\0', MAX_BUFFER);

	while(1) {
		
        char buffer[MAX_BUFFER];
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

                if(strncmp(stopWord, buffer + start, i - start) == 0) {

                    
                    // fprintf(stdoutFile, "%s\n", stopWord);
                    // fflush(stdoutFile);
                    buffer[i - strlen(stopWord)] = '\0';
                    break;
                }
            }
        }
        

        if (bytes + strlen(inputStr) > lenInput ) {
            lenInput = lenInput * 2 + 1;
            char * tempStr = (char *) malloc(lenInput * sizeof(char));
            memset(tempStr, '\0', lenInput);
            strncpy(tempStr, inputStr, strlen(inputStr) );
            strcat(tempStr, buffer);
            inputStr = realloc(inputStr, lenInput);
            strcpy(inputStr, tempStr);
            strcat(inputStr, "\0");
            free(tempStr);

        } else {

            strcat(inputStr, buffer);
            strcat(inputStr, "\0");
        }
        
        if( i < bytes) {
            break;
        }
        
	}
	return inputStr;
}