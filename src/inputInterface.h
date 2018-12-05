#ifndef INPUT_INTERFACE_H
#define INPUT_INTERFACE_H

#include <stdint.h>

#include "basicStructs.h"
#include "queryStructs.h"

#define BINARY_FILE 1
#define ASCII_FILE 2

typedef struct Table {
        myint_t columns;
        myint_t rows;

        uint64_t ** content;
} table;

typedef struct Arguments {
        char rPath[500];
        char sPath[500];

        table * rTable;
        table * sTable;

        myint_t colS;
        myint_t colR;

        myint_t type;
        char outPath[500];
} arguments;

arguments * readArguments(myint_t argc, char *argv[]);

table * readTable(char * filePath, myint_t fileType);
myint_t readAsciiTable(table * t, FILE * inputFile);
myint_t readBinTable(table * t, FILE * inputFile);
myint_t applyLine(table * t, myint_t whichCol, char * buffer);
relation * extractRelation(uint64_t * col, uint64_t size);
void printTable(table * t);
void printRelation(relation * r);
void freeTable(table * t);

myint_t writeList(headResult * head, char * outPath);

headResult * radixHashJoin(relation * rRel, relation * sRel, char * switched);


#endif