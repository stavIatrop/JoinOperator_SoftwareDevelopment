#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "basicStructs.h"
#include "viceFunctions.h"

reorderedR * reorderRelation(relation * r, uint32_t *hash1)
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
	uint32_t size = r->size, i, *hash_values, *hist;

	hash_values = malloc(size * sizeof(uint32_t));

        if (*hash1==0)
	{
		*hash1 = FindNextPrime(floor(1.05 * (size * sizeof(tuple) / CACHE_SIZE)) + 1);
		hist = Hash1(r,hash1,hash_values);
	}
	else
	{
		uint32_t groups, fix;
		hist = malloc(*hash1 * sizeof(uint32_t));
		DoTheHash(r,*hash1,hist,hash_values,&groups,&fix);
	}

	printf("hash1 is %d\n", *hash1);
//	printf("final hash1 is %d\n",*hash1);

        uint32_t buckets = *hash1;

//	for (i = 0; i < *hash1; i++) printf("number of objects for bucket %d is %d\n",i,hist[i]);
//	printf("Created hist and hash_values.\n");

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
//	printf("Created Psum\n");

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

//	printf("Sorted r\n");
	R->rel = r;
	free(hist);
	free(helpPSum);
	free(members);
	free(hash_values);
	return R;
}
