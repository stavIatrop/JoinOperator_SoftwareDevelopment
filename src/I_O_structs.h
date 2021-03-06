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
void ConstructInput(Input * , char * );
void FreeInput(Input *);

relationsheepArray InitializeRelSheepArray(myint_t numRels);
void FillRelArray(relationsheepArray * , Input *);
myint_t FillRel(relationsheep * , FILE *);
void FreeRelArray(relationsheepArray );
void PrintRelArray(relationsheepArray *, FILE *);


#endif