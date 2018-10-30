#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

int digitsOfInt(uint32_t integer)
{
        int count = 1;
        while((integer = integer/10) > 0)
                count++;
        return count;
}


int main(int argc, char const *argv[])
{
        if(argc != 4) {
                printf("Wrong arguments: ./createTable <cols> <rows> <fileName>\n");
                return -1;
        }

        uint32_t cols = atoi(argv[1]);
        uint32_t rows = atoi(argv[2]);
        int digits, numb;
        srand(time(NULL)); 
        char * firstLine = (char *) malloc(50 * sizeof(char));
        strcpy(firstLine, argv[1]);
        strcat(firstLine, " ");
        strcat(firstLine, argv[2]);
        strcat(firstLine, "\n");

        char * buffer = (char *) malloc(rows * 11 + 1);
        FILE *f = fopen(argv[3], "w");

        fprintf(f, "%s", firstLine);
        free(firstLine);
        for(int whichCol = 0; whichCol < cols; whichCol++) {
                for(int whichRow = 0; whichRow < rows; whichRow++) {
                        numb = rand();
                        digits = digitsOfInt(numb);
                        char * tempString = (char *) malloc((digits +1) * sizeof(char));
                        sprintf(tempString, "%d", numb);
                        if(whichRow == 0) {
                                strcpy(buffer, tempString);
                        }
                        else {
                                strcat(buffer, tempString);
                        }
                        free(tempString);

                        if(whichRow != rows - 1) {
                                strcat(buffer, "|");
                        }
                }
                fprintf(f, "%s\n", buffer);
        }

        free(buffer);
        fclose(f);
        return 0;
}
