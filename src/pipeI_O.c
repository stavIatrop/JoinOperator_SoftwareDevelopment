#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "pipeI_O.h"
#include "queryStructs.h"
#include "checksumInterface.h"

char * readFromPipe(const char * stopWord) {


	myint_t lenInput = MAX_BUFFER + 1;
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

        myint_t end = 0, start = 0;
        myint_t i;
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

myint_t numDigits(myint_t n) {

  myint_t counter = 0;
  
  while(n != 0) {

    n = n / 10;
    counter++;
  }
  return counter;
}

void writePipe(checksum * cs) {

    char ** CSStrings = (char **) malloc(cs->numbOfChecksums * sizeof(char *));
    myint_t csLength = 0;

    for(myint_t i = 0; i < cs->numbOfChecksums; i++) {

        if (cs->checksums[i] == 0) {

            CSStrings[i] = (char *) malloc((strlen("NULL ") + 1) * sizeof(char));
            memset(CSStrings[i], '\0', strlen("NULL ") + 1);
            strcpy(CSStrings[i], "NULL ");
            strcat(CSStrings[i], "\0");
            csLength += strlen("NULL ");

        } else { 

            myint_t digits = numDigits(cs->checksums[i]) + 1;       //plus 1 for space character
            CSStrings[i] = (char *) malloc((digits + 1) * sizeof(char));
            memset(CSStrings[i], '\0', (digits + 1));
            sprintf(CSStrings[i], "%ld ", cs->checksums[i]);
            strcat(CSStrings[i], "\0");
            csLength += digits;
        }
        if( i == cs->numbOfChecksums - 1 ) {
            CSStrings[i][strlen(CSStrings[i]) - 1] = '\n';
        }
        
    }

    char * messageInABottle = (char *) malloc( (csLength + 1) * sizeof(char));
    memset(messageInABottle, '\0', csLength + 1);

    for(myint_t i = 0; i < cs->numbOfChecksums; i++) {

        strcat(messageInABottle, CSStrings[i]);
        free(CSStrings[i]);
    }
    free(CSStrings);
    strcat(messageInABottle, "\0");

    myint_t bSent = 0;
    myint_t bWritten;
    while( (bWritten = write(STDOUT_FILENO, messageInABottle + bSent, strlen(messageInABottle) - bSent)) > 0 ) {


      if(bWritten < 0) {
        perror("Write");
        exit(EXIT_FAILURE);
      }
      bSent += bWritten;
      
      if(bSent == strlen(messageInABottle))
        break;
    }

    free(messageInABottle);

    return;

    

}