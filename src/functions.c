#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "basicStructs.h"
#include "viceFunctions.h"
#include "jobScheduler.h"

pthread_mutex_t histMutex;
pthread_cond_t histCond;
int histsCompleted = 0;

myint_t Hash1_2(myint_t key, myint_t len) {
   return key % len;
}

myint_t FindNextPower(myint_t number)
{
	myint_t power = 1;
	while (power < number) power<<=1;
	return power;
}

double IdenticalityTest(relation *r) //described in viceFunctions.h
{
	
	myint_t size = r->size, limit = size/100, sames=0;
	if (size==0) return 0.0;
                fflush(stderr);
	myint_t value = r->tuples[0].key, newValue;
	
	srand(time(NULL));
	
	for (myint_t i=0; i<limit; i++)
	{
		newValue = r->tuples[rand() % size].key;
		if (newValue == value) sames++;
		else value = newValue;
	}
	return sqrt((double)sames / limit);
}


void rSwap(tuple * t,myint_t *hash_values, myint_t i, myint_t j)
{
	myint_t tempi;
	myint_t tempu;
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

void histogramJob(void *limits)
{
	myint_t hash1 = ((content *) limits)->hash1;
	myint_t *hist = malloc(hash1 * sizeof(myint_t)), *hash_values = malloc((((content *) limits)->to - ((content *) limits)->from) * sizeof(myint_t));
	if (hist ==NULL)
    {
    	perror("Simon says: malloc failed");
        exit(1);
    }
    tuple *t = ((content *) limits)->t;
	myint_t i, to = ((content *) limits)->to, value, from = ((content *) limits)->from;
	for (i = 0; i < hash1; i++) hist[i] = 0;

    for (i = from; i < to; i++)
    {
            value = Hash1_2(t[i].key,hash1);
            hash_values[i - from] = value;
            hist[value] = hist[value] + 1;
    }
	((content *) limits)->hist = hist;
	((content *) limits)->hash_values = hash_values;

	pthread_mutex_lock(&histMutex);
        histsCompleted += 1;
        if(histsCompleted == NUMB_OF_THREADS) {
                pthread_cond_signal(&histCond);
        }
        pthread_mutex_unlock(&histMutex);

	return;
}

myint_t DoTheHash(relation *r, myint_t hash1, myint_t *hist, myint_t *hash_values, myint_t *max, char trivial)
{	//hash1 is the number of buckets in the hashing, hist is the histogram and hash_values the hash value for each key
	
	*max = 0;
	myint_t maxBucketSize = floor(AVAILABLE_CACHE_SIZE / sizeof(tuple)), bad=0;

	pthread_mutex_init(&histMutex, 0);
	pthread_cond_init(&histCond, 0);
	histsCompleted = 0;
	content **contents = malloc(NUMB_OF_THREADS * sizeof(content));
	for (myint_t i=0; i<NUMB_OF_THREADS; i++)
	{
		contents[i]=malloc(sizeof(content));
	}
	for (myint_t i=0; i<NUMB_OF_THREADS; i++)
	{
		contents[i]->from = i*(r->size/NUMB_OF_THREADS);
		contents[i]->to = (i!=NUMB_OF_THREADS-1 ? (i+1)*(r->size/NUMB_OF_THREADS) : r->size);
		contents[i]->t = r->tuples;
		contents[i]->hash1 = hash1;

		struct Job *job = malloc(sizeof(struct Job));
		job->function = &histogramJob;
		job->argument = (void *) contents[i];
		writeOnQueue(job);
	}

	//Barrier for all the jobs to finish
    pthread_mutex_lock(&histMutex);
    while(histsCompleted != NUMB_OF_THREADS)
    {
            pthread_cond_wait(&histCond, &histMutex);
    }
    pthread_mutex_unlock(&histMutex);

	myint_t cur = 0, i;
	
	for (i=0; i<hash1; i++) hist[i] = contents[0]->hist[i];

	for (i = contents[0]->from; i < contents[0]->to; i++)
	{
        hash_values[cur++] = contents[0]->hash_values[i-contents[0]->from];
	}

	free(contents[0]->hash_values);
	free(contents[0]->hist);
	free(contents[0]);

	for (myint_t j=1; j<NUMB_OF_THREADS; j++)
	{
		for (i=0; i<hash1; i++)
		{
			hist[i]+= contents[j]->hist[i];
		}
		for (i = contents[j]->from; i < contents[j]->to; i++)
    	{
            hash_values[cur++] = contents[j]->hash_values[i-contents[j]->from];
    	}
		free(contents[j]->hash_values);
		free(contents[j]->hist);
		free(contents[j]);
	}
	free(contents);

	pthread_mutex_destroy(&histMutex);
	pthread_cond_destroy(&histCond);

	if (trivial!=0) return 0; //below here is only for calculating the best hash1. If we just want to do the hash, trivial!=0.

	myint_t value;

    for (i=0; i<hash1; i++)
    {
		value = hist[i];
		if (value > maxBucketSize)
		{
			bad++;
			*max += value;
		}
    }
	return bad;
}


myint_t *Hash1(relation *r,myint_t *hash1, myint_t *hash_values)
{
	myint_t size = r->size, *hist, prevBad, max, beginning, maxBucketSize = floor(AVAILABLE_CACHE_SIZE / sizeof(tuple)), nextPower;
	hist = malloc(*hash1 * sizeof(myint_t));
	if (hist ==NULL)
    {
            perror("Wrong arguments");
            exit(1);
    }

	double identicality=IdenticalityTest(r);

	myint_t bad = DoTheHash(r,*hash1,hist,hash_values,&max,0);
	beginning = *hash1;
	prevBad = bad;

	while (bad > 0)
    {
		nextPower = (myint_t)log2(FindNextPower(max/maxBucketSize)+1);
		if (log2(*hash1) + nextPower > floor(log2(size)) || log2(*hash1) + nextPower > floor(log2(BUCKET_MEMORY_LIMIT)) || max <= 1.1 * identicality * size)
		{		
			//the last check in the f is because identical keys cannot be separated.
			if (*hash1==beginning) return hist;
			*hash1 = beginning;
        	hist = realloc(hist,*hash1 * sizeof(myint_t));
			if (hist ==NULL)
   			{
    		        perror("Wrong arguments");
	                exit(1);
	        }

	        DoTheHash(r,*hash1,hist,hash_values,&max,1);
			return hist;
		}
		*hash1 = (*hash1) << nextPower;
        if (bad != prevBad) beginning = *hash1;
		prevBad = bad;
        hist = realloc(hist,*hash1 * sizeof(myint_t));
		if (hist ==NULL)
    	{
                perror("Wrong arguments");
                exit(1);
        }
        bad = DoTheHash(r,*hash1,hist,hash_values,&max,0);
    }

	return hist;
}

