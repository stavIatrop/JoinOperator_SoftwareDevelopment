#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "basicStructs.h"
#include "viceFunctions.h"

void CheckR(relation *r, uint32_t hash1)
{
	if (r->size==0)
	{
		printf("relation is empty\n");
		return;
	}
	printf("Checking R, assuming the hash function is key %% length.\n");
	uint32_t size = r->size, current=0, value;
	for (uint32_t i=0; i<size; i++)
	{
		value = r->tuples[i].key % hash1;
		if (value < current)
		{
			printf("----------FAIL: R has not been sorted properly\n");
			return;
		}
		current = value;
	}
	printf("----------PASS: R has been sorted properly\n");
}

int main(int argc, char const *argv[])
{
	uint32_t hash1=0, size = 10000000;
	relation *r;
	r = malloc(sizeof(relation));
	r->tuples = malloc(size * sizeof(tuple));
	r->size=size;
	uint32_t random;
	srand(time(NULL));
	reorderedR *R;


	printf("\n\n------------Test 1: %d random keys:------------\n\n",size);
	for (uint32_t i=0; i<size; i++)
	{
		random = rand();
		r->tuples[i].key = random;
		r->tuples[i].payload = 837376;
	}
	printf("Entering reordering function\n");
	R = reorderRelation(r,&hash1);
	printf("Exited reordering function\n");
	CheckR(r,hash1);
        hash1=0;


	printf("\n\n------------Test 2: keys are 0 - %d (reversed):------------\n\n",size-1);
        for (uint32_t i=0; i<size; i++)
        {
                //if (i%100 == 0) r.tuples[i].key = 526;
                r->tuples[i].key = size -1 -i;
                r->tuples[i].payload = 837376;
        }
        printf("Entering reordering function\n");
        R = reorderRelation(r,&hash1);
        printf("Exited reordering function\n");
	CheckR(r,hash1);
        hash1=0;


        printf("\n\n------------Test 3: %d identical keys:------------\n\n",size);
	random = rand();
        for (uint32_t i=0; i<size; i++)
        {
                r->tuples[i].key = random;
                r->tuples[i].payload = 837376;
        }
        printf("Entering reordering function\n");
        R = reorderRelation(r,&hash1);
        printf("Exited reordering function\n");
	CheckR(r,hash1);
        hash1=0;


        printf("\n\n------------Test 4: %d keys with 1%% of them being identical:------------\n\n",size);
        for (uint32_t i=0; i<size; i++)
        {
                random = rand();
                if (i%100 == 0) r->tuples[i].key = 526;
                else r->tuples[i].key = random;
		r->tuples[i].payload = 837376;
        }
        printf("Entering reordering function\n");
        R = reorderRelation(r,&hash1);
        printf("Exited reordering function\n");
	CheckR(r,hash1);
        hash1=0;


        printf("\n\n------------Test 5: %d keys with 10%% of them being identical:------------\n\n",size);
        for (uint32_t i=0; i<size; i++)
        {
                random = rand();
                if (i%10 == 0) r->tuples[i].key = 526;
                else r->tuples[i].key = random;
                r->tuples[i].payload = 837376;
        }
        printf("Entering reordering function\n");
        R = reorderRelation(r,&hash1);
        printf("Exited reordering function\n");
	CheckR(r,hash1);
        hash1=0;


        printf("\n\n------------Test 6: Empty relation:------------\n\n");
	r->size = 0;
        printf("Entering reordering function\n");
        R = reorderRelation(r,&hash1);
        printf("Exited reordering function\n");
	CheckR(r,hash1);
	r->size = size;
        hash1=0;


        printf("\n\n------------Test 7: %d keys with 2 groups of 10%% and 20%% of them being identical within each group:------------\n\n",size);
        for (uint32_t i=0; i<size; i++)
        {
                random = rand();
                if (i%10 == 0) r->tuples[i].key = 526;
		else if (i%10 == 1 || i%10 == 2) r->tuples[i].key = 527;
                else r->tuples[i].key = random;
                r->tuples[i].payload = 837376;
        }
        printf("Entering reordering function\n");
        R = reorderRelation(r,&hash1);
        printf("Exited reordering function\n");
        CheckR(r,hash1);
        hash1=0;


	printf("\n\n------------Test 8: %d keys that only have values [0-9]:------------\n\n",size);
        for (uint32_t i=0; i<size; i++)
        {
                random = rand();
                r->tuples[i].key = random % 10;
                r->tuples[i].payload = 837376;
        }
        printf("Entering reordering function\n");
        R = reorderRelation(r,&hash1);
        printf("Exited reordering function\n");
        CheckR(r,hash1);


	printf("\n\n------------Test 9: Sorting the S after Test 8, with the keys being random:------------\n\n");
        for (uint32_t i=0; i<size; i++)
        {
                random = rand();
                r->tuples[i].key = random;
                r->tuples[i].payload = 837376;
        }
        printf("Entering reordering function\n");
        R = reorderRelation(r,&hash1);
        printf("Exited reordering function\n");
        CheckR(r,hash1);
	hash1=0;


	printf("\n\n------------Test 10: %d keys with some of them sneakily being in the same bucket:------------\n\n",size);
        for (uint32_t i=0; i<size; i++)
        {
                random = rand();
		uint32_t primeNumber1 = 331;
		uint32_t primeNumber2 = FindNextPrime(primeNumber1 + 1);
		uint32_t primeNumber3 = FindNextPrime(primeNumber2 + 1);
		uint32_t primeNumber4 = FindNextPrime(primeNumber3 + 1);
                if (i%primeNumber1 == 0) r->tuples[i].key = i;
		else if (i%primeNumber2 == 0) r->tuples[i].key = i;
		else if (i%primeNumber3 == 0) r->tuples[i].key = i;
		else if (i%primeNumber4 == 0) r->tuples[i].key = i;
		//else if (i%10==0) r->tuples[i].key = 0;
                //else if (i%10==1 || i%10==2) r->tuples[i].key = 1;
                else r->tuples[i].key = random;
                r->tuples[i].payload = 837376;
        }
        printf("Entering reordering function\n");
        R = reorderRelation(r,&hash1);
        printf("Exited reordering function\n");
        CheckR(r,hash1);
	hash1 = 0;


	printf("\n\n------------Test 11: %d keys with some of them sneakily being in the same bucket and 0.5%% being identical:------------\n\n",size);
        for (uint32_t i=0; i<size; i++)
        {
                random = rand();
                uint32_t primeNumber1 = 331;
                uint32_t primeNumber2 = FindNextPrime(primeNumber1 + 1);
                uint32_t primeNumber3 = FindNextPrime(primeNumber2 + 1);
                uint32_t primeNumber4 = FindNextPrime(primeNumber3 + 1);
                if (i%primeNumber1 == 0) r->tuples[i].key = i;
                else if (i%primeNumber2 == 0) r->tuples[i].key = i;
                else if (i%primeNumber3 == 0) r->tuples[i].key = i;
                else if (i%primeNumber4 == 0) r->tuples[i].key = i;
                else if (i%500==0) r->tuples[i].key = 1;
                //else if (i%10==1 || i%10==2) r->tuples[i].key = 1;
                else r->tuples[i].key = random;
                r->tuples[i].payload = 837376;
        }
        printf("Entering reordering function\n");
        R = reorderRelation(r,&hash1);
        printf("Exited reordering function\n");
        CheckR(r,hash1);
        hash1 = 0;


	printf("\n\n------------Test 9: 0xFFFFFFFF-1 (max number) keys:------------\n\n");
        printf("Not implemented yet.\n");
	/*size = 1000000000;
	r.size = size;
	printf("size has been set to %d\n", size);
	for (uint32_t i=0; i<size; i++)
        {
		if (i<0) printf("AAAAAAAAA");
                random = rand();
                r.tuples[i].key = random;
                r.tuples[i].payload = 837376;
        }
        printf("Entering reordering function\n");
        R = reorderRelation(&r,&hash1);
        printf("Exited reordering function\n");
	CheckR(r,hash1);*/


	free(R->rel->tuples);
	free(R->rel);
	free(R);
        return 0;
}

