#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inputInterface.h"
#include "basicStructs.h"

table * readTable(char * filePath) {
        char * tempLine = NULL;

        table * t = (table *) malloc(sizeof(table));
        if(t == NULL) {
                perror("Failed to malloc table");
                return NULL;
        }

        FILE * inputFile  = fopen(filePath, "r");
        if(inputFile == NULL) {
                perror("Failed to open file");
                return NULL;
        }

        char * firstLine = NULL;
        size_t len = 0;
        if(getline(&firstLine, &len, inputFile) != -1) {
               t->columns = atoi(strtok(firstLine, " \n"));
               t->rows = atoi(strtok(NULL, " \n"));
        }
        else {
                perror("Failed to read first line");
                return NULL;
        }
        free(firstLine);

        t->content = (int32_t **) malloc(t->columns * sizeof(int32_t *));
        if(t->content == NULL) {
                perror("Failed to malloc columns");
                return NULL;
        }
        for(int32_t whichCol = 0; whichCol < t->columns; whichCol++) {
                t->content[whichCol] = (int32_t *) malloc(t->rows * sizeof(int32_t));
                if(t->content[whichCol] == NULL) {
                        perror("Failed to malloc row of column");
                        return NULL;
                }
        }

        for(int32_t whichCol = 0; whichCol < t->columns; whichCol++) {
                len = 0;
                tempLine = NULL;

                if(getline(&tempLine, &len, inputFile) == -1) {
                        perror("Failed to read table column");
                        return NULL;
                }
                if(applyLine(t, whichCol, tempLine) == -1) {
                        printf("Failed to apply line\n");
                        return NULL;
                }

                free(tempLine);          
        }
        fclose(inputFile);

        return t;

}

int applyLine(table * t, int32_t whichCol, char * buffer) {
        char * tok = strtok(buffer, "|\n");
        for(int32_t whichLine = 0; whichLine < t->rows; whichLine++) {
                if(tok == NULL) {
                        perror("Strtok failed");
                        return -1;
                }
                t->content[whichCol][whichLine] = atoi(tok);
                tok = strtok(NULL, "|\n");
        }
        return 0;
}

relation * extractRelation(table * t, int column) {
        relation * r = (relation *) malloc(sizeof(relation));

        r->size = t->rows;
        r->tuples = (tuple *) malloc(t->rows * sizeof(tuple));

        for(int whichRow = 0; whichRow < t->rows; whichRow++) {
                r->tuples[whichRow].payload = whichRow;
                r->tuples[whichRow].key = t->content[column][whichRow];
        }

        return r;
}

//DEBUG PURPOSES
void printTable(table * t) {
        printf("  <<< Printing table>>>\n");
        for(int i = 0; i < t->columns; i++) {
                printf("Column %d: ", i);
                for(int j = 0; j < t->rows; j++) {
                       printf("%d \t", t->content[i][j]); 
                }
                printf("\n");
        }
        printf("\n");
}

void printRelation(relation * r) {
        printf("  <<< Printing relation of size %d >>>\n", r->size);
        for(int i = 0; i < r->size; i++) {
                printf("(%d, %d)\n", r->tuples[i].payload, r->tuples[i].key);
        }
        printf("\n");
}
