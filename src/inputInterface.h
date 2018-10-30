#ifndef INPUT_INTERFACE_H
#define INPUT_INTERFACE_H

#include <stdint.h>

#include "basicStructs.h"

typedef struct Table {
        uint32_t columns;
        uint32_t rows;

        int32_t ** content;
} table;

table * readTable(char * filePath);
int applyLine(table * t, int32_t whichCol, char * buffer);
relation * extractRelation(table * t, int column);
void printTable(table * t);
void printRelation(relation * r);


#endif