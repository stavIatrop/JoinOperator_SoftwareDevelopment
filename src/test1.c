#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "basicStructs.h"

void CheckR(relation r, uint32_t hash1)
{
	printf("Checking R, assuming the hash function is key %% length.\n");
	uint32_t size = r.size, current=0, value;
	for (uint32_t i=0; i<size; i++)
	{
		value = r.tuples[i].key % hash1;
		if (value < current)
		{
			printf("R has not been sorted properly\n");
			return;
		}
		current = value;
	}
	printf("R has been sorted properly\n");
}

int main(int argc, char const *argv[])
{
	uint32_t hash1, size = 10000000;
	relation r;
	r.tuples = malloc(size * sizeof(tuple));
	r.size=size;
	uint32_t random;
	srand(time(NULL));
	reorderedR *R;


	printf("\n\n------------Test 1: 10.000.000 random keys:------------\n\n");
	for (uint32_t i=0; i<size; i++)
	{
		random = rand();
		r.tuples[i].key = random;
		r.tuples[i].payload = 837376;
	}
	printf("Entering stupidly named reordering function\n");
	R = reordereRelation(&r,&hash1);
	printf("Exited stupidly named reordering function\n");
	CheckR(r,hash1);


	printf("\n\n------------Test 2: keys are 1 - 10.000.000:------------\n\n");
        for (uint32_t i=0; i<size; i++)
        {
                //if (i%100 == 0) r.tuples[i].key = 526;
                r.tuples[i].key = i;
                r.tuples[i].payload = 837376;
        }
        printf("Entering stupidly named reordering function\n");
        R = reordereRelation(&r,&hash1);
        printf("Exited stupidly named reordering function\n");
	CheckR(r,hash1);


        printf("\n\n------------Test 3: 10.000.000 identical keys:------------\n\n");
	random = rand();
        for (uint32_t i=0; i<size; i++)
        {
                r.tuples[i].key = random;
                r.tuples[i].payload = 837376;
        }
        printf("Entering stupidly named reordering function\n");
        R = reordereRelation(&r,&hash1);
        printf("Exited stupidly named reordering function\n");
	CheckR(r,hash1);


        printf("\n\n------------Test 4: 10.000.000 keys with 1%% of them being identical:------------\n\n");
        for (uint32_t i=0; i<size; i++)
        {
                random = rand();
                if (i%100 == 0) r.tuples[i].key = 526;
                else r.tuples[i].key = random;
		r.tuples[i].payload = 837376;
        }
        printf("Entering stupidly named reordering function\n");
        R = reordereRelation(&r,&hash1);
        printf("Exited stupidly named reordering function\n");
	CheckR(r,hash1);


        printf("\n\n------------Test 5: 10.000.000 keys with 10%% of them being identical:------------\n\n");
        for (uint32_t i=0; i<size; i++)
        {
                random = rand();
                if (i%10 == 0) r.tuples[i].key = 526;
                else r.tuples[i].key = random;
                r.tuples[i].payload = 837376;
        }
        printf("Entering stupidly named reordering function\n");
        R = reordereRelation(&r,&hash1);
        printf("Exited stupidly named reordering function\n");
	CheckR(r,hash1);


        printf("\n\n------------Test 6: Empty relation:------------\n\n");
	r.size = 0;
        printf("Entering stupidly named reordering function\n");
        R = reordereRelation(&r,&hash1);
        printf("Exited stupidly named reordering function\n");
	r.size = size;
	CheckR(r,hash1);


        printf("\n\n------------Test 7: 0xFFFFFFFF (max number) keys:------------\n\n");
        printf("Not implemented yet\n");
	/*for (uint32_t i=0; i<size; i++)
        {
                random = rand();
                if (i%10 == 0) r.tuples[i].key = 526;
                else r.tuples[i].key = random;
                r.tuples[i].payload = 837376;
        }
        printf("Entering stupidly named reordering function\n");
        R = reordereRelation(&r,&hash1);
        printf("Exited stupidly named reordering function\n");
	CheckR(r,hash1);*/


	free(R->rel->tuples);
        return 0;
}

