#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

myint_t digitsOfInt(myint_t integer)
{
        myint_t count = 1;
        while((integer = integer/10) > 0)
                count++;
        return count;
}


myint_t main(myint_t argc, char const *argv[])
{
        if(argc != 5) {
                printf("Wrong arguments: ./createTable <cols> <rows> <fileName> ascii/bin\n");
                return -1;
        }

        myint_t cols = atoi(argv[1]);
        myint_t rows = atoi(argv[2]);
        myint_t digits, numb;
        srand(time(NULL));
        char * buffer;
        char * firstLine = (char *) malloc(50 * sizeof(char));
        strcpy(firstLine, argv[1]);
        strcat(firstLine, " ");
        strcat(firstLine, argv[2]);
        strcat(firstLine, "\n");

        FILE *f = fopen(argv[3], "w");
        fprintf(f, "%s", firstLine);
        free(firstLine);

        if(strcmp(argv[4], "bin") != 0) {
                buffer = (char *) malloc(rows * 11 + 1);
                for(myint_t whichCol = 0; whichCol < cols; whichCol++) {
                        for(myint_t whichRow = 0; whichRow < rows; whichRow++) {
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
        }
        else {
                uint64_t * array = (uint64_t *) calloc(rows * cols, sizeof(uint64_t));
                if(array == NULL) {
                        perror("Failed to allocate buffer");
                        return -1;
                }
                for(myint_t whichInt = 0; whichInt < rows * cols; whichInt++) {
                        array[whichInt] = rand();	//RANDOM
                        //printf("%d | %lu\n", whichInt, array[whichInt]);
                        //array[whichInt] = 7;		//SAME
			//array[whichInt] = whichInt;	//ORDERED
                }
                fwrite(array, sizeof(uint64_t), rows * cols, f);
        }
        
        fclose(f);
        return 0;
}
