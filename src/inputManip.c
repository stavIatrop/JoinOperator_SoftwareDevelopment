#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "inputInterface.h"
#include "basicStructs.h"

arguments * readArguments(int argc, char *argv[]) {

        /*if(argc != 6) {
                return NULL;
        }*/

        char charType[500];
        charType[0] = '!';
        arguments * args = (arguments *) malloc(sizeof(arguments));
        args->rPath[0]='!';
        args->sPath[0]='!';
        args->colR = -1;
        args->colS = -1;
        int opt;
        while((opt = getopt(argc, argv, "R:S:r:s:t:")) != -1)
        {
                switch(opt)
                {
                        case 'R':
                                strcpy(args->rPath, optarg);
                                break;
                        case 'S':
                                strcpy(args->sPath, optarg);
                                break;
                        case 'r':
                                args->colR = atoi(optarg);
                                break;
                        case 's':
                                args->colS = atoi(optarg);
                                break;
                        case 't':
                                strcpy(charType, optarg);
                                break;
                        case '?':
                                return NULL;
                }
        }

        if((args->rPath[0] == '!') || (args->sPath[0] == '!') || (args->colR == -1) || (args->colS == -1) || (charType[0] == '!')) {
                return NULL;
        }

        if(strcmp(charType, "binary") == 0) {
                args->type = BINARY_FILE;
        }
        else if(strcmp(charType, "ascii") == 0) {
                args->type = ASCII_FILE;
        }
        else {
                return NULL;
        }

        return args;
}

table * readTable(char * filePath, int fileType) {
        
        size_t len;
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
                t->content[whichCol] = (int32_t *) calloc(t->rows, sizeof(int32_t));
                if(t->content[whichCol] == NULL) {
                        perror("Failed to malloc row of column");
                        return NULL;
                }
        }

        if(fileType == ASCII_FILE) {
                if(readAsciiTable(t, inputFile) != 0) {
                        return NULL;
                }
        }
        else if(fileType == BINARY_FILE) {
                if(readBinTable(t, inputFile) != 0) {
                        return NULL;
                }
        }

        fclose(inputFile);

        return t;

}

int readAsciiTable(table * t, FILE * inputFile) {
        char * tempLine = NULL;
        size_t len;
        for(int32_t whichCol = 0; whichCol < t->columns; whichCol++) {
                len = 0;
                tempLine = NULL;

                if(getline(&tempLine, &len, inputFile) == -1) {
                        perror("Failed to read table column");
                        return -1;
                }
                if(applyLine(t, whichCol, tempLine) == -1) {
                        printf("Failed to apply line\n");
                        return -1;
                }

                free(tempLine);          
        }
        return 0;
}

int readBinTable(table * t, FILE * inputFile) {
        for(int32_t whichCol = 0; whichCol < t->columns; whichCol++) {
                if(fread(t->content[whichCol], sizeof(int32_t), t->rows, inputFile) != t->rows) {
                        perror("Failed to read binary line");
                        return -1;
                }
        }
        return 0;
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
