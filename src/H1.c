#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "basicStructs.h"

#define CACHE_SIZE 256000

uint32_t Hash1_1(void *key, uint32_t len) {			//FNV Hash, but it's not made to be used this way so it may suck.
    unsigned char *p = key;
    unsigned h = 0x811c9dc5;
    int i;

    for ( i = 0; i < 4; i++ )
      h = ( h ^ p[i] ) * 0x01000193;

   return h % len;
}

uint32_t Hash1_2(int32_t key, uint32_t len) {                    //Pure genius.
   return key % len;
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

uint32_t *Hash1(relation *r,uint32_t *hash1, uint32_t *hist)
{
	uint32_t maxBucketSize = CACHE_SIZE / sizeof(tuple);
	printf("maxBucketSize is %d\n", maxBucketSize);
	uint32_t size = r->size, *hash_values,i,value;
	hash_values = malloc(size * sizeof(uint32_t));
	uint32_t max1 = 0, max2=0;
	char flag = 1;

	for (i = 0; i < *hash1; i++) hist[i] = 0;

        for (i = 0; i < size; i++)
        {
		//value = Hash1_2(r->tuples[i].key,*hash1);
		value = Hash1_1((void *) &(r->tuples[i].key),*hash1);
                hash_values[i] = value;
                hist[value] = hist[value] + 1;
		if (hist[value] > maxBucketSize)
		{
			flag = 0;
			break;
		}
		else if (hist[value]>max1) max1 = hist[value];
        }

	if (flag==1)
	{
		printf("The max amount of entries in a bucket is %d\n",max1);
		return hash_values;
	}

	printf("Hash1 was not good enough\n");
	for (i = 0; i < *hash1; i++) hist[i] = 0;

        for (i = 0; i < size; i++)
        {
                //value = Hash1_1((void *) &(r->tuples[i].key),*hash1);
		value = Hash1_2(r->tuples[i].key,*hash1);
                hash_values[i] = value;
                hist[value] = hist[value] + 1;
		if (hist[value]>max2) max2 = hist[value];
        }
	printf("The max amount of entries in a bucket is %d\n",max2);
	return hash_values;
}

reorderedR * reordereRelation(relation * r, uint32_t *hash1)
{
	uint32_t size = r->size, value, i, *hash_values;
	uint32_t *hist = malloc(*hash1 * sizeof(uint32_t)), max;

	*hash1 = floor(1.1 * (size * sizeof(tuple) / CACHE_SIZE));
        uint32_t buckets = *hash1;

	printf("hash1 is %d\n", *hash1);
	hash_values = Hash1(r,hash1,hist);

	for (i = 0; i < *hash1; i++)
	{
//		printf("number of objects for bucket %d is %d\n",i,hist[i]);
	}
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

	uint32_t current_bucket = psum->psum[0].h1Res, *members = malloc(buckets * sizeof(uint32_t));

	for (i=0; i<buckets; i++)
	{
		members[i] = 0;
	}

	for (i = 0; i < size; i++)
	{
//		printf("%d, current_bucket: %d\n",i,current_bucket);
		while (members[current_bucket] == hist[current_bucket])
                {
                        current_bucket++;
                        i+=members[current_bucket];
			//if (members[current_bucket] == hist[current_bucket]) i--;
                }
		if (i>=size) break;
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
