#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "basicStructs.h"

int main(int argc, char const *argv[])
{
	uint32_t hash1, size = 100000000;
	relation r;
	r.tuples = malloc(size * sizeof(tuple));
	r.size=size;
	uint32_t random;
	srand(time(NULL));
	for (uint32_t i=0; i<size; i++)
	{
		random = rand();
		//if (i%100 == 0) r.tuples[i].key = 526;
		r.tuples[i].key = random;
		r.tuples[i].payload = 837376;
	}
	printf("Entering stupidly named reordering function\n");
	reorderedR *R = reordereRelation(&r,&hash1);
	printf("Exited stupidly named reordering function\n");
	//for (uint32_t i=0; i<size; i++) printf("Key %d is %d\n", i, R->rel->tuples[i].key);
	free(R->rel->tuples);
        return 0;
}

