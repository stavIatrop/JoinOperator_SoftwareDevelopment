#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "basicStructs.h"

#define FNV_PRIME_32 16777619
#define FNV_OFFSET_32 2166136261U
#define CACHE_SIZE 256000

uint32_t Hash1_1(void *key, uint32_t len) {			//FNV Hash, but it's not made to be used this way so it may suck.
    unsigned char *p = key;
    unsigned h = 0x811c9dc5;
    int i;

    for ( i = 0; i < 4; i++ )
      h = ( h ^ p[i] ) * 0x01000193;

   return h % len;
}

uint32_t Fnv32(const char *s, uint32_t len)	//found in http://ctips.pbworks.com/w/page/7277591/FNV%20Hash, slightly altered
{
    uint32_t hash = FNV_OFFSET_32, i;
    for(i = 0; i < 4; i++)
    {
        hash = hash ^ (s[i]); // xor next byte into the bottom of the hash
        hash = hash * FNV_PRIME_32; // Multiply by prime number found to work well
    }
    return hash % len;
}

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


char IdenticalityTest(relation *r)
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

uint32_t *Hash1(relation *r,uint32_t *hash1, uint32_t *hash_values)
{
	uint32_t maxBucketSize = CACHE_SIZE / sizeof(tuple);
	printf("maxBucketSize is %d\n", maxBucketSize);
	uint32_t size = r->size, *hist,i,j,value, initial = *hash1;
	hist = malloc(*hash1 * sizeof(uint32_t));

	uint32_t max1 = 0, max2=0, min1 = size, min2 = size, repeats=0;
	char flag;

	if (IdenticalityTest(r))
	{
		printf("Considered repetitive\n");
		*hash1 = 1.1*initial + 1; //skips the while loop
	}

	if (initial > (uint32_t) (0xFFFFFFFF * 0.9))
        {
                printf("Considered too big\n");
                *hash1 = 1.1*initial + 1; //skips the while loop
        }

	/*while (*hash1 <= (uint32_t)(1.1*initial))
	{
		repeats++;
		if (repeats%10==0) if (IdenticalityTest(r))
        	{
        	        printf("Considered repetitive\n");
        	        break; //skips the while loop
        	}

		flag = 1;
		for (i = 0; i < *hash1; i++) hist[i] = 0;

	        for (i = 0; i < size; i++)
	        {
			value = Hash1_2(r->tuples[i].key,*hash1);
			//value = Hash1_1((void *) &(r->tuples[i].key),*hash1);
			//value = Fnv32((const char *) &(r->tuples[i].key),*hash1);
        	        printf("value is %d, i is %d\n", value, i);
			hash_values[i] = value;
			printf("Got you\n");
			fflush(stdout);
	                hist[value] = hist[value] + 1;
			printf("Got you\n");
			if (hist[value] > maxBucketSize)
			{
				flag = 0;
				break;
			}
        	}
		printf("AA, %d\n", flag);
		if (flag==0)
		{
			for (j=0;j<10;j++) *hash1 = FindNextPrime(*hash1);
			hist = realloc(hist,*hash1 * sizeof(uint32_t));
			continue;
		}

		for (i=0; i<*hash1; i++)
		{
			if (hist[i]>max1) max1 = hist[i];
	                if (hist[i]<min1) min1 = hist[i];
		}

		printf("The min amount of entries in a bucket is %d\n",min1);
		printf("The max amount of entries in a bucket is %d\n",max1);
		return hash_values;
	}*/

	*hash1 = initial;
	//hist = realloc(hist,initial * sizeof(uint32_t));

	for (i = 0; i < *hash1; i++) hist[i] = 0;

        for (i = 0; i < size; i++)
        {
                value = Hash1_2(r->tuples[i].key,*hash1);
                //value = Hash1_1((void *) &(r->tuples[i].key),*hash1);
                //value = Fnv32((const char *) &(r->tuples[i].key),*hash1);
	//	printf("is is %d, value is %d\n",i,value);
                hash_values[i] = value;
	//	printf("Got you\n");
                hist[value] = hist[value] + 1;
        }

	for (i=0; i<*hash1; i++)
        {
                if (hist[i]>max1) max1 = hist[i];
                if (hist[i]<min1) min1 = hist[i];
        }

        printf("The min amount of entries in a bucket is %d\n",min1);
        printf("The max amount of entries in a bucket is %d\n",max1);
        return hist;

}

reorderedR * reordereRelation(relation * r, uint32_t *hash1)
{
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
