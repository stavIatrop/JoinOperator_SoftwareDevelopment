#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "basicStructs.h"
#include "viceFunctions.h"

reorderedR * reorderRelation(relation * r, myint_t *hash1)
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
	myint_t size = r->size, i, *hash_values, *hist;

	hash_values = malloc(size * sizeof(myint_t));	//stores the hash value of every key
	if (hash_values==NULL)
	{
		perror("Wrong arguments");
                exit(1);
	}

        if (*hash1==FIRST_REORDERED) //hash1 contains the bucket size, or FIRST_REORDERED if it has to be calculated
	{
		*hash1 = FindNextPower(floor(ERROR_MARGIN * (size * sizeof(tuple) / AVAILABLE_CACHE_SIZE)) + 1); //ERROR_MARGIN ==1.05 to fit
		hist = Hash1(r,hash1,hash_values); //hist contains the histogram
	}
	else
	{
		*hash1 = pow(2,*hash1);
		myint_t max;
		hist = malloc(*hash1 * sizeof(myint_t));
		if (hist ==NULL)
		{
			perror("Wrong arguments");
                	exit(1);
		}
		DoTheHash(r,*hash1,hist,hash_values,&max,1);
	}

        myint_t buckets = *hash1;

	for (i = 0; i < *hash1; i++) if (hist[i] == 0) buckets--; //psum only contains entries for the hash values with at least 1 element

	reorderedR *R = malloc(sizeof(reorderedR));
	if (R ==NULL)
        {
               	perror("Wrong arguments");
                exit(1);
        }
	pSumArray *psum;
	psum = &(R->pSumArr);
	psum->psumSize = buckets;
	psum->psum = malloc(buckets * sizeof(pSumTuple));
	if (psum->psum ==NULL)
        {
                perror("Wrong arguments");
                exit(1);
        }


	myint_t j = 0, prevJ;
	myint_t *helpPSum = malloc((*hash1) * sizeof(myint_t)); //like psum, but contains entries for all hash values. Needed to sort
	if (helpPSum ==NULL)
        {
                perror("Wrong arguments");
                exit(1);
        }

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
	}
							//members counts how many elements have been sorted so far for each hash value
	myint_t current_bucket = psum->psum[0].h1Res, current = 0, *members = malloc(*hash1 * sizeof(myint_t));
	if (members ==NULL)
        {
                perror("Wrong arguments");
                exit(1);
        }


	for (i=0; i<*hash1; i++)
	{
		members[i] = 0;
	}

	char flag = 1;

	for (i = 0; i < size; i++)	//This sorting is done in O(n) time
	{
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
			while (hash_values[helpPSum[hash_values[i]] + members[hash_values[i]]] == hash_values[i])
			{
				members[hash_values[i]] = members[hash_values[i]] + 1;
			}
			members[hash_values[i]] = members[hash_values[i]] + 1;
			rSwap(r->tuples,hash_values,i,helpPSum[hash_values[i]] + members[hash_values[i]] - 1);
		}
		members[current_bucket] = members[current_bucket] + 1;
	}

	char a = 0;
	while (*hash1)
	{
		*hash1 >>= 1;
		a++;
	}	//returns *hash1=n, the number of least important bits used by the hash function
	*hash1 = a-1;
	R->rel = r;
	free(hist);
	free(helpPSum);
	free(members);
	free(hash_values);
	return R;
}
