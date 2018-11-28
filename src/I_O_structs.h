#ifndef I_O_STRUCTS_H
#define I_O_STRUCTS_H

#include "queryStructs.h"

typedef struct InputFilesListNode {

	char * filename;
	struct InputFilesListNode *next;
	
}InputNode;

typedef struct InputList {

	InputNode *head;
	InputNode *tail;
	myint_t numNodes;

}Input;


Input * InitializeInput();
void AddInputNode(Input * , char *);
void FreeInput(Input *);

relationsheepArray InitializeRelSheepArray(myint_t numRels);
void FillRelArray(relationsheepArray * , Input * );
int FillRel(relationsheep * , FILE * );
void FreeRelArray(relationsheepArray );


#endif