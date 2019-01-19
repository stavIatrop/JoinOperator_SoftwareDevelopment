#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "basicStructs.h"
#include "viceFunctions.h"
#include "jobScheduler.h"

pthread_mutex_t *workMutex;

pthread_mutex_t constructionMutex;
pthread_cond_t constructionCond;
int constructionsCompleted = 0;

void constructionJob(void *Blues)
{
	blueprints *blues = (blueprints *) Blues;
	myint_t *hash_values = blues->hash_values;
	myint_t *hist = blues->hist;
	tuple *tuples = blues->tuples;
	myint_t to = blues->to, from = blues->from;
	tuple *newR = blues->newR;
	myint_t *helpPsum = blues->helpPsum;
	myint_t hash1 = blues->hash1;
	myint_t size = blues->size;

	myint_t temp;

	for (myint_t i=from; i<to; i++)
	{
		pthread_mutex_lock(&(workMutex[hash_values[i]]));
		temp = hist[hash_values[i]];
		hist[hash_values[i]] --;
		pthread_mutex_unlock(&(workMutex[hash_values[i]]));
		if (hash_values[i]<hash1-1)
		{
			newR[helpPsum[hash_values[i]+1] - temp].key = tuples[i].key;
			newR[helpPsum[hash_values[i]+1] - temp].payload = tuples[i].payload;
		}
		else
		{
			newR[size - temp].key = tuples[i].key;
			newR[size - temp].payload = tuples[i].payload;
		}
	}

	pthread_mutex_lock(&constructionMutex);
    constructionsCompleted += 1;
    if(constructionsCompleted == NUMB_OF_THREADS) {
            pthread_cond_signal(&constructionCond);
    }
    pthread_mutex_unlock(&constructionMutex);

	return;
}

reorderedR * reorderRelation(relation * r, myint_t *hash1)
{
	if (r==NULL) return NULL;

	if (r->size==0)
	{
		reorderedR *R = malloc(sizeof(reorderedR));
		R->rel = malloc(sizeof(relation));
		R->rel->size=0;
		R->rel->tuples=NULL;
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

	workMutex = malloc((*hash1) * sizeof(pthread_mutex_t));
	for (myint_t i=0; i<*hash1; i++)
	{
		pthread_mutex_init(&(workMutex[i]), 0);
	}

        myint_t buckets = *hash1;

	for (i = 0; i < *hash1; i++) if (hist[i] == 0) buckets--; //psum only contains entries for the hash values with at least 1 element

	reorderedR *R = malloc(sizeof(reorderedR));	
	if (R ==NULL)
        {
               	perror("Wrong arguments");
                exit(1);
        }

    //fprintf(stderr,"FFF\n");

    R->rel = malloc(sizeof(relation));
    R->rel->tuples = malloc((r->size)*sizeof(tuple));
    if (R->rel->tuples ==NULL)
        {
               	perror("Wrong arguments");
                exit(1);
        }
	R->rel->size = r->size;

	//fprintf(stderr,"EEE\n");

	pSumArray *psum;
	psum = &(R->pSumArr);
	psum->psumSize = buckets;
	psum->psum = malloc(buckets * sizeof(pSumTuple));
	if (psum->psum ==NULL)
        {
                perror("Wrong arguments");
                exit(1);
        }


	myint_t j = 0, prevJ=0;
	myint_t *helpPSum = malloc((*hash1) * sizeof(myint_t)); //like psum, but contains entries for all hash values. Needed to sort
	if (helpPSum ==NULL)
        {
                perror("Wrong arguments");
                exit(1);
        }


	myint_t temp = 0;
	while (hist[j]==0)
	{
		helpPSum[j]=temp;
		j++;
	}
	if (prevJ < *hash1 -1) temp += hist[j];
	else temp = size;
	psum->psum[0].h1Res = j;
	psum->psum[0].offset = 0;
	helpPSum[j] = 0;
	prevJ = j;
	j++;

	for (i = 1; i < buckets; i++)
	{
		while (j<*hash1)
    	{
    		if (hist[j]!=0) break;
        	helpPSum[j]=temp;
        	j++;
    	}
    	if (j==*hash1) break;
		psum->psum[i].h1Res = j;
        psum->psum[i].offset = psum->psum[i-1].offset + hist[prevJ];
		helpPSum[j] = psum->psum[i].offset;
		prevJ = j++;
		if (i==buckets-1) break;
		temp += hist[prevJ];
	}
	for (;j<*hash1;j++) helpPSum[j]=size;

	blueprints *blues = malloc(NUMB_OF_THREADS*sizeof(blueprints));
	pthread_mutex_init(&constructionMutex, 0);
	pthread_cond_init(&constructionCond, 0);
	constructionsCompleted = 0;

	for (i=0; i<NUMB_OF_THREADS; i++)
	{
		blues[i].from = i*(r->size/NUMB_OF_THREADS);
		blues[i].to = (i!=NUMB_OF_THREADS-1 ? (i+1)*(r->size/NUMB_OF_THREADS) : r->size);
		blues[i].hash1 = *hash1;
		blues[i].size = size;
		blues[i].tuples = r->tuples;
		blues[i].hash_values = hash_values;
		blues[i].helpPsum = helpPSum;
		blues[i].hist = hist;
		blues[i].newR = R->rel->tuples;

		struct Job *job = malloc(sizeof(struct Job));
		job->function = &constructionJob;
		job->argument = (void *) &(blues[i]);
		writeOnQueue(job);
	}

	pthread_mutex_lock(&constructionMutex);
    while(constructionsCompleted != NUMB_OF_THREADS) {
            pthread_cond_wait(&constructionCond, &constructionMutex);
    }
    pthread_mutex_unlock(&constructionMutex);

    free(blues);

	pthread_mutex_destroy(&constructionMutex);
	pthread_cond_destroy(&constructionCond);

	

	for (i=0; i<*hash1; i++) pthread_mutex_destroy(&(workMutex[i]));
	free(workMutex);

	char a = 0;
	while (*hash1)
	{
		*hash1 >>= 1;
		a++;
	}	//returns *hash1=n, the number of least important bits used by the hash function
	*hash1 = a-1;
	free(hist);
	free(helpPSum);
	free(hash_values);

	return R;
}
