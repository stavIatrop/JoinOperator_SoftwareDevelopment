#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main(void){

	FILE * inputfile = fopen("/home/stav/Desktop/Project/ioannidisproject/small/r0", "rb");
	if(inputfile == NULL) {
            perror("Failed to open file\n");
            return -1;
        }
	uint64_t  * numRows;
	numRows = malloc(5 * sizeof(uint64_t));

	if(fread((void *)numRows, sizeof(uint64_t), 5, inputfile) != 5) {
        perror("Failed to read binary metadata");
        return -1;
    }
    
    printf("%ld\n", numRows[0] );
    printf("%ld\n", numRows[1] );
    printf("%ld\n", numRows[2] );
    printf("%ld\n", numRows[3] );
    printf("%ld\n", numRows[4] );

    fclose(inputfile);

    FILE * stdoutFile = fopen("stdoutFile", "w");

    fprintf(stdoutFile, "start\n");
    fclose(stdoutFile);
    return 0;
}