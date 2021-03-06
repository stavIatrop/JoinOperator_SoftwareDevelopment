#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "I_O_structs.h"
#include "queryStructs.h"
#include "getStats.h"

Input * InitializeInput() {

	Input * input = (Input *) malloc(sizeof(Input));

	input->head = NULL;
	input->tail = NULL;
	input->numNodes = 0;
	return input;
}



void AddInputNode(Input * input, char *file) {


	InputNode * newNode = (InputNode *) malloc(sizeof(InputNode));
	newNode->filename = (char *) malloc( (strlen(file) + 1) * sizeof(char));
	memset(newNode->filename, '\0', strlen(file) + 1 );
	strcpy(newNode->filename, file);
	strcat(newNode->filename, "\0");
	newNode->next = NULL;

	input->numNodes = input->numNodes + 1;

	if( input->head == NULL) {

		input->head = newNode;
		input->tail = input->head;

		return;
	}

    input->tail->next = newNode;
    input->tail = input->tail->next;

	return;
}

void ConstructInput(Input * input, char * inputStr) {

	myint_t end = 0, start = 0;
    myint_t i;
    for(i = 0; i < strlen(inputStr); i++) {

            if( inputStr[i] == '\n') {
                start = end;
                end = i + 1;

                char * filename = (char *) malloc((i - start + 1) * sizeof(char));
                memset(filename, '\0', i - start + 1);
                strncpy(filename, inputStr + start, i - start);

                
                AddInputNode(input,filename);
                
                free(filename);
            }
    }
    return;
}


void FreeInput(Input * input){

	InputNode * temp;
	for(myint_t i = 0; i < input->numNodes; i++) {
		temp = input->head;
		input->head = temp->next;
		free(temp->filename);
		free(temp);
	}
	free(input);
	return;
}

relationsheepArray InitializeRelSheepArray(myint_t numRels){

	relationsheepArray relArray;
	relArray.rels = (relationsheep *) malloc(numRels * sizeof(relationsheep));
	relArray.numOfRels = numRels;
	return relArray;
}

void FillRelArray(relationsheepArray * relArray, Input * input) {

	InputNode * temp;
	temp = input->head;

	for (myint_t i = 0; i < input->numNodes; i++) {
		
		FILE * inputFile  = fopen(temp->filename, "rb");
		
		if(inputFile == NULL) {
            perror("Failed to open file2\n");
            return;
        }
		
		if ( FillRel(&(relArray->rels[i]), inputFile ) == -1) {
			perror("Failed to read binary metadata");
			return;
		}
		temp = temp->next;
		fclose(inputFile);
	}
	return;
}

myint_t FillRel(relationsheep * rel, FILE * inputFile) {


	if(fread((void *) &(rel->rows), sizeof(myint_t), 1, inputFile) != 1) {
        perror("Failed to read binary metadata");
        return -1;
    }
    if(fread((void *) &(rel->cols), sizeof(myint_t), 1, inputFile) != 1) {
        perror("Failed to read binary metadata");
        return -1;
    }
    rel->statsArray = (stats *) malloc(rel->cols * sizeof(stats));
	
	
    rel->pointToCols = (myint_t **) malloc(rel->cols * sizeof(myint_t *));
    for(myint_t i = 0; i < rel->cols; i++) {

    	rel->pointToCols[i] = (myint_t *) malloc(rel->rows * sizeof(myint_t));
    	if(fread((void *) rel->pointToCols[i], sizeof(myint_t), rel->rows, inputFile) != rel->rows) {
        	perror("Failed to read binary column");
        	return -1;
    	}

		FillStatsArray( rel->pointToCols[i], &(rel->statsArray[i]), rel->rows );
    }
    return 1;

}

void PrintRelArray(relationsheepArray * relArray, FILE * fp) {


	for(myint_t i = 0; i < relArray->numOfRels; i++) {

		for(myint_t j = 0; j < relArray->rels[i].cols; j++ ) {

			for(myint_t k = 0; k < relArray->rels[i].rows; k++) {

				char * s = malloc(10 * sizeof(char));
				sprintf(s, "%ld", relArray->rels[i].pointToCols[j][k]);
				fprintf(fp, "%s|", s );
				fflush(fp);
				free(s);
			}
			fprintf(fp, "\n");
			fflush(fp);

		}
	}
	return;

}

void FreeRelArray(relationsheepArray relArray) {

	for(myint_t i = 0; i < relArray.numOfRels; i++) {

		for(myint_t j = 0; j < relArray.rels[i].cols; j++) {

			free(relArray.rels[i].pointToCols[j]);
			free(relArray.rels[i].statsArray[j].distinctArray);
		}
		free(relArray.rels[i].pointToCols);
		free(relArray.rels[i].statsArray);
	}
	free(relArray.rels);
	return;
}