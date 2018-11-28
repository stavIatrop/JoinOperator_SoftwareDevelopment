#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "I_O_structs.h"
#include "queryStructs.h"

Input * InitializeInput() {

	Input * input = (Input *) malloc(sizeof(Input));

	input->head = NULL;
	input->tail = NULL;
	input->numNodes = 0;
	return input;
}

void AddInputNode(Input * input, char *file) {


	if( input->head == NULL) {

		input->head = (InputNode *) malloc(sizeof(InputNode));
		input->head->filename = (char *) malloc(sizeof(file) * sizeof(char));
		strcpy(input->head->filename, file);
		input->head->next = NULL;
		input->tail = input->head;
		input->numNodes = input->numNodes + 1;
		return;
	}

	input->tail->next = (InputNode *) malloc(sizeof(InputNode));
	input->tail = input->tail->next;
	input->tail->filename = (char *) malloc(sizeof(file) * sizeof(char));
	strcpy(input->tail->filename, file);
	input->tail->next = NULL;
	input->numNodes = input->numNodes + 1;
	return;
}

void FreeInput(Input * input){

	InputNode * temp;
	for(int i = 0; i < input->numNodes; i++) {
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

	for (int i = 0; i < input->numNodes; ++i) {
		
		FILE * inputFile  = fopen(temp->filename, "rb");
		if(inputFile == NULL) {
            perror("Failed to open file\n");
            return;
        }

		FillRel(&(relArray->rels[i]), inputFile );
		temp = temp->next;
		fclose(inputFile);
	}
	return;
}

int FillRel(relationsheep * rel, FILE * inputFile) {


	if(fread((void *) &(rel->rows), sizeof(myint_t), 1, inputFile) != 1) {
        perror("Failed to read binary metadata");
        return -1;
    }
    if(fread((void *) &(rel->cols), sizeof(myint_t), 1, inputFile) != 1) {
        perror("Failed to read binary metadata");
        return -1;
    }

    rel->pointToCols = (myint_t **) malloc(rel->cols * sizeof(myint_t *));
    for(int i = 0; i < rel->cols; i++) {

    	rel->pointToCols[i] = (myint_t *) malloc(rel->rows * sizeof(myint_t));
    	if(fread((void *) rel->pointToCols[i], sizeof(myint_t), rel->rows, inputFile) != rel->rows) {
        	perror("Failed to read binary column");
        	return -1;
    	}
    }
    return 1;

}


void FreeRelArray(relationsheepArray relArray) {

	free(relArray.rels);
	return;
}