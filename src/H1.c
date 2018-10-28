#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "basicStructs.h"

#define CACHE_SIZE 256000

uint32_t Hash1_2(int32_t key, uint32_t len) {                    //Pure genius.
   return key % len;
}


uint32_t FindNextPrime(uint32_t candidate)
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
}


char IdenticalityTest(relation *r)		//not used, but cool
{
	uint32_t size = r->size, searchSize = size/10, limit = (CACHE_SIZE / sizeof(tuple))/10 , sames=0;
	int32_t value = r->tuples[0].key, newValue;
	for (uint32_t i=0; i<searchSize; i++)
	{
		newValue = r->tuples[rand() % size].key;
		if (newValue == value)
		{
			sames++;
			if (sames==limit) return 1;
		}
		value = newValue;
	}
	return 0;
}


void rSwap(tuple * t,uint32_t *hash_values, uint32_t i, uint32_t j)
{
//	printf("swapping %d with %d\n",i,j);
	int32_t temp;
	temp = t[i].key;
	t[i].key = t[j].key;
	t[j].key = temp;

	temp = t[i].payload;
        t[i].payload = t[j].payload;
        t[j].payload = temp;

	temp = hash_values[i];
        hash_values[i] = hash_values[j];
        hash_values[j] = temp;
}

uint32_t DoTheHash(relation *r, uint32_t hash1, uint32_t *hist, uint32_t *hash_values, uint32_t *groups)
{
	*groups = 0;
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
		if (hist[i] > 2*maxBucketSize)
		{
			reduction += hist[i];
			(*groups)++;
		}
        }
	printf("Reduction is %d and the groups are %d\n",reduction, *groups);
	return reduction;
}


uint32_t *Hash1(relation *r,uint32_t *hash1, uint32_t *hash_values)
{
	uint32_t size = r->size, *hist,i,value, groups;
	hist = malloc(*hash1 * sizeof(uint32_t));

	uint32_t reduction = DoTheHash(r,*hash1,hist,hash_values,&groups);

	if (reduction > 0)
	{
		*hash1 = FindNextPrime(floor(1.05 * ((size-reduction) * sizeof(tuple) / CACHE_SIZE) + groups));
		hist = realloc(hist,*hash1 * sizeof(uint32_t));

		for (i = 0; i < *hash1; i++) hist[i] = 0;

        	for (i = 0; i < size; i++)
        	{
                	value = Hash1_2(r->tuples[i].key,*hash1);
                	hash_values[i] = value;
                	hist[value] = hist[value] + 1;
        	}
	}

        return hist;
}

reorderedR * reordereRelation(relation * r, uint32_t *hash1)
{

	if (r==NULL) return NULL;

	if (r->size==0)
	{
		reorderedR *R = malloc(sizeof(reorderedR));
		R->rel = r;
		R->pSumArr.psumSize = 0;
		R->pSumArr.psum = NULL;
		return R;
	}
	uint32_t size = r->size, value, i, *hash_values, candidate, *hist;

	candidate = floor(1.05 * (size * sizeof(tuple) / CACHE_SIZE)) + 1;
        *hash1 = FindNextPrime(candidate);
//	if (*hash1 > size) *hash1 = size;
	hash_values = malloc(size * sizeof(uint32_t));

	printf("initial hash1 is %d\n", *hash1);
	hist = Hash1(r,hash1,hash_values);
	printf("final hash1 is %d\n",*hash1);

        uint32_t buckets = *hash1;

//	for (i = 0; i < *hash1; i++) printf("number of objects for bucket %d is %d\n",i,hist[i]);
	printf("Created hist and hash_values.\n");

	for (i = 0; i < *hash1; i++) if (hist[i] == 0) buckets--;

	reorderedR *R = malloc(sizeof(reorderedR));
	pSumArray *psum;
	psum = &(R->pSumArr);
	psum->psumSize = buckets;
	psum->psum = malloc(buckets * sizeof(pSumTuple));

	uint32_t j = 0, prevJ;
	uint32_t *helpPSum = malloc((*hash1) * sizeof(uint32_t));
	while (hist[j]==0)
	{
		helpPSum[j]=0;
		j++;
	}
	psum->psum[0].h1Res = j;
	psum->psum[0].offset = 0;
	helpPSum[j] = 0;
	prevJ = j;
	j++;

	for (i = 1; i < buckets; i++)
	{
		while (hist[j]==0)
        	{
                	helpPSum[j]=0;
                	j++;
        	}
		psum->psum[i].h1Res = j;
        	psum->psum[i].offset = psum->psum[i-1].offset + hist[prevJ];
		helpPSum[j] = psum->psum[i].offset;
		prevJ = j++;
//		printf("j = %d, prevJ = %d\n",j,prevJ);
	}

//	for (i=0; i<buckets; i++) printf("PSUM: bucket %d starts at %d\n",psum->psum[i].h1Res,psum->psum[i].offset);
	printf("Created Psum\n");

	uint32_t current_bucket = psum->psum[0].h1Res, current = 0, *members = malloc(*hash1 * sizeof(uint32_t));

	for (i=0; i<*hash1; i++)
	{
		members[i] = 0;
	}

	char flag = 1;

	for (i = 0; i < size; i++)
	{
//		printf("%d, current_bucket: %d\n",i,current_bucket);
		while (members[current_bucket] == hist[current_bucket])
                {
                        current_bucket = psum->psum[++current].h1Res;
                        i+=members[current_bucket];
			if (i>=size)
			{
				flag = 0;
				break;
			}
                }
		if (flag==0) break;
		while (hash_values[i] != current_bucket)
		{
//			printf("i is %d, hash_value is %d, helpPSum is %d and members is %d\n",i,hash_values[i],helpPSum[hash_values[i]],members[hash_values[i]]);
			while (hash_values[helpPSum[hash_values[i]] + members[hash_values[i]]] == hash_values[i])
			{
				members[hash_values[i]] = members[hash_values[i]] + 1;
			}
			members[hash_values[i]] = members[hash_values[i]] + 1;
			rSwap(r->tuples,hash_values,i,helpPSum[hash_values[i]] + members[hash_values[i]] - 1);
		}
		members[current_bucket] = members[current_bucket] + 1;
	}

	printf("Sorted r\n");
	R->rel = r;
	free(hist);
	free(helpPSum);
	free(members);
	free(hash_values);
	return R;
}
