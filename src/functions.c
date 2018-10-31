#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "basicStructs.h"
#include "viceFunctions.h"

#define ACCEPTANCE_LIMIT 4

uint32_t Hash1_2(int32_t key, uint32_t len) {                    //Pure genius.
   return key % len;
}


/*uint32_t FindNextPower(uint32_t candidate)
{
	uint32_t i, limit;
	char flag;
	if (candidate % 2 == 0) candidate++;
	while (1)
	{
		flag = 1;
		limit = floor(sqrt(candidate));
		for (i=3; i<=limit; i+=2)
		{
			if (candidate % i == 0)
			{
				candidate+=2;
				flag = 0;
				break;
			}
		}
		if (flag==0) continue;
		return candidate;
	}
}*/

uint32_t FindNextPower(uint32_t number)
{
	uint32_t power = 1;
	while (power < number) power<<=1;
	return power;
}

double IdenticalityTest(relation *r)
{
	uint32_t size = r->size, sames=0;
	int32_t value = r->tuples[0].key, newValue;
	for (uint32_t i=0; i<size; i++)
	{
		newValue = r->tuples[rand() % size].key;
		if (newValue == value) sames++;
		value = newValue;
	}
	return (double) sames / size;
}


void rSwap(tuple * t,uint32_t *hash_values, uint32_t i, uint32_t j)
{
//	printf("swapping %d with %d\n",i,j);
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

uint32_t DoTheHash(relation *r, uint32_t hash1, uint32_t *hist, uint32_t *hash_values, uint32_t *groups, uint32_t *bad, uint32_t *max)
{
	*groups = 0;
	*bad = 0;
	*max = 0;
	uint32_t maxBucketSize = floor(CACHE_SIZE / sizeof(tuple));
	printf("maxBucketSize is %d\n", maxBucketSize);
	uint32_t i, size = r->size, value, reduction=0;
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
			(*bad)++;
			*max += value;
		}
		if (value > ACCEPTANCE_LIMIT*maxBucketSize)
		{
			reduction += value;
			(*groups)++;
		}
        }
	printf("Max size of bucket within acceptable limits is %d and the number of bad buckets is %d\n",*max,*bad);
	printf("Reduction is %d and the groups are %d\n",reduction, *groups);
	return reduction;
}


uint32_t *Hash1(relation *r,uint32_t *hash1, uint32_t *hash_values)
{
	uint32_t size = r->size, *hist, groups, bad, prevBad= r->size,max, prevMax = 0, beginning, repeats=0;
	hist = malloc(*hash1 * sizeof(uint32_t));
	double identicality=0;

	uint32_t reduction = DoTheHash(r,*hash1,hist,hash_values,&groups,&bad,&max);

	printf("bad is %d\n", bad);
	if (bad > 0)
	{
		identicality = sqrt(IdenticalityTest(r));
		printf("Identicality is %f\n", identicality);
		*hash1 = FindNextPower(floor(ERROR_MARGIN * (size*(1-identicality) * sizeof(tuple) / CACHE_SIZE)) + 1);
		hist = realloc(hist,*hash1 * sizeof(uint32_t));
                DoTheHash(r,*hash1,hist,hash_values,&groups,&bad,&max);
	}
	/*if (reduction > 0)
	{
		*hash1 = FindNextPower(floor(ERROR_MARGIN * ((size-reduction) * sizeof(tuple) / CACHE_SIZE) + groups));
		printf("Reduction was made, current hash1 is %d\n",*hash1);
		hist = realloc(hist,*hash1 * sizeof(uint32_t));
		DoTheHash(r,*hash1,hist,hash_values,&groups,&bad,&max);
	}*/
	beginning = *hash1;

	printf("max is %d, *hash1 is %d\n",max,*hash1);
	while (max > 1.1 * identicality * size)
        {
		if (*hash1 > size || *hash1 > BUCKET_MEMORY_LIMIT)
		{
			*hash1 = beginning;
        	        hist = realloc(hist,*hash1 * sizeof(uint32_t));
	                DoTheHash(r,*hash1,hist,hash_values,&groups,&bad,&max);
			return hist;
		}
                if (bad != prevBad) beginning = *hash1;
		prevBad = bad;
		prevMax = max;
		*hash1 <<= 1;
		printf("Fix attempt was made, current hash1 is %d\n",*hash1);
                hist = realloc(hist,*hash1 * sizeof(uint32_t));
                DoTheHash(r,*hash1,hist,hash_values,&groups,&bad,&max);
        }

        return hist;
}
