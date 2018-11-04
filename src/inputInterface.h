#ifndef INPUT_INTERFACE_H
#define INPUT_INTERFACE_H

#include <stdint.h>

#include "basicStructs.h"

#define BINARY_FILE 1
#define ASCII_FILE 2

//typedef struct HeadResult headResult;

typedef struct Table {
        uint32_t columns;
        uint32_t rows;

        int32_t ** content;
} table;

typedef struct Arguments {
        char rPath[500];
        char sPath[500];

        table * rTable;
        table * sTable;

        int32_t colS;
        int32_t colR;

        int type;
        char outPath[500];
} arguments;

arguments * readArguments(int argc, char *argv[]);

table * readTable(char * filePath, int fileType);
int readAsciiTable(table * t, FILE * inputFile);
int readBinTable(table * t, FILE * inputFile);
int applyLine(table * t, int32_t whichCol, char * buffer);
relation * extractRelation(table * t, int column);
void printTable(table * t);
void printRelation(relation * r);
void freeTable(table * t);

int writeList(headResult * head, char * outPath);

headResult * radixHashJoin(table * rTable, table * sTable, int32_t colR, int32_t colS);


#endif