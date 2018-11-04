#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "basicStructs.h"
#include "viceFunctions.h"

uint32_t Hash1_2(int32_t key, uint32_t len) {                    //Pure genius.
   return key % len;
}

uint32_t FindNextPower(uint32_t number)
{
	uint32_t power = 1;
	while (power < number) power<<=1;
	return power;
}

double IdenticalityTest(relation *r)
{
	uint32_t size = r->size, sames=0, limit = size/10;
	int32_t value = r->tuples[0].key, newValue;
	for (uint32_t i=0; i<limit; i++)
	{
		newValue = r->tuples[rand() % size].key;
		if (newValue == value) sames++;
		value = newValue;
	}
	return (double) sqrt((double)sames / limit);
}


void rSwap(tuple * t,uint32_t *hash_values, uint32_t i, uint32_t j)
{
	int32_t tempi;
	uint32_t tempu;
	tempi = t[i].key;
	t[i].key = t[j].key;
	t[j].key = tempi;

	tempu = t[i].payload;
        t[i].payload = t[j].payload;
        t[j].payload = tempu;

	tempu = hash_values[i];
        hash_values[i] = hash_values[j];
        hash_values[j] = tempu;
}

uint32_t DoTheHash(relation *r, uint32_t hash1, uint32_t *hist, uint32_t *hash_values, uint32_t *max)
{
	*max = 0;
	uint32_t maxBucketSize = floor(AVAILABLE_CACHE_SIZE / sizeof(tuple));
	uint32_t i, size = r->size, value, bad=0;
	for (i = 0; i < hash1; i++) hist[i] = 0;

        for (i = 0; i < size; i++)
        {
                value = Hash1_2(r->tuples[i].key,hash1);
                hash_values[i] = value;
                hist[value] = hist[value] + 1;
        }

        for (i=0; i<hash1; i++)
        {
		value = hist[i];
		if (value > maxBucketSize)
		{
			bad++;
			*max += value;
		}
        }
	printf("Max size of bucket not within acceptable limits is %d and the number of bad buckets is %d\n",*max,bad);
	return bad;
}


uint32_t *Hash1(relation *r,uint32_t *hash1, uint32_t *hash_values)
{
	uint32_t size = r->size, *hist, prevBad, max, beginning, maxBucketSize = floor(AVAILABLE_CACHE_SIZE / sizeof(tuple)), nextPower;
	printf("maxBucketSize is %d\n", maxBucketSize);
	hist = malloc(*hash1 * sizeof(uint32_t));
	double identicality=0;

	uint32_t bad = DoTheHash(r,*hash1,hist,hash_values,&max);
	beginning = *hash1;
	prevBad = bad;

	while (1)
        {
		nextPower = (uint32_t)log2(FindNextPower(max/maxBucketSize)+1);
                printf("Next power is %d\n", nextPower);
		if (log2(*hash1) + nextPower > floor(log2(size)) || log2(*hash1) + nextPower > floor(log2(BUCKET_MEMORY_LIMIT)) || max <= 1.1 * identicality * size)
		{
			*hash1 = beginning;
        	        hist = realloc(hist,*hash1 * sizeof(uint32_t));
	                bad = DoTheHash(r,*hash1,hist,hash_values,&max);
			return hist;
		}
		*hash1 = (*hash1) << nextPower;
                if (bad != prevBad) beginning = *hash1;
		prevBad = bad;
		printf("Fix attempt was made, current hash1 is %d\n",*hash1);
                hist = realloc(hist,*hash1 * sizeof(uint32_t));
                bad = DoTheHash(r,*hash1,hist,hash_values,&max);
        }
}
