#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "basicStructs.h"
#include "viceFunctions.h"
#include "jobScheduler.h"
#include "bitVector.h"
#include "getStats.h"
#include "queryStructs.h"

#define BETTERER 100

pthread_mutex_t histMutex;
pthread_cond_t histCond;
int histsCompleted = 0;

myint_t curmem=0;

indexArray *FetchFromIndexWarehouse(myint_t rel, myint_t col, myint_t *hash1)
{
	fprintf(stderr,"Fetching %lu, %lu\n\n",rel,col);

	if (indexWarehouse == NULL)
	{
		indexWarehouse = malloc(sizeof(wares));
		indexWarehouse ->size = 0;
		indexWarehouse->rel = malloc(500*sizeof(myint_t));
		indexWarehouse->col = malloc(500*sizeof(myint_t));
		indexWarehouse->hash1 = malloc(500*sizeof(myint_t));
		indexWarehouse->indexes = malloc(500*sizeof(indexArray *));
		return NULL;
	}

	for (myint_t i=0; i<indexWarehouse->size; i++)
	{
		if (indexWarehouse->rel[i]==rel && indexWarehouse->col[i]==col)
		{
			*hash1 = indexWarehouse->hash1[i];
			fprintf(stderr,"FETCHED! %lu, %lu\n\n",rel,col);
			return indexWarehouse->indexes[i];
		}
	}
	return NULL;
}

reorderedR *FetchFromReWarehouse(myint_t rel, myint_t col, myint_t *hash1)
{
	fprintf(stderr,"Fetching ror %lu, %lu\n\n",rel,col);

	if (reWarehouse == NULL)
	{
		reWarehouse = malloc(sizeof(reWares));
		reWarehouse ->size = 0;
		reWarehouse->rel = malloc(500*sizeof(myint_t));
		reWarehouse->col = malloc(500*sizeof(myint_t));
		reWarehouse->hash1 = malloc(500*sizeof(myint_t));
		reWarehouse->arrays = malloc(500*sizeof(reorderedR *));
		return NULL;
	}

	if (*hash1==FIRST_REORDERED)
	{
		for (myint_t i=0; i<reWarehouse->size; i++)
		{
			if (reWarehouse->rel[i]==rel && reWarehouse->col[i]==col)
			{
				*hash1 = reWarehouse->hash1[i];
				fprintf(stderr,"FETCHED ror! %lu, %lu\n\n",rel,col);
				return reWarehouse->arrays[i];
			}
		}
	}
	else
	{
		for (myint_t i=0; i<reWarehouse->size; i++)
		{
			if (reWarehouse->rel[i]==rel && reWarehouse->col[i]==col && reWarehouse->hash1[i]==*hash1)
			{
				fprintf(stderr,"FETCHED ror! %lu, %lu\n\n",rel,col);
				return reWarehouse->arrays[i];
			}
		}
	}
	return NULL;
}

void AddToIndexWarehouse(myint_t rel, myint_t col, myint_t hash1, indexArray *indexes)
{
	if (indexWarehouse == NULL)
	{
		indexWarehouse = malloc(sizeof(wares));
		indexWarehouse ->size = 0;
		indexWarehouse->rel = malloc(500*sizeof(myint_t));
		indexWarehouse->col = malloc(500*sizeof(myint_t));
		indexWarehouse->hash1 = malloc(500*sizeof(myint_t));
		indexWarehouse->indexes = malloc(500*sizeof(reorderedR*));
		return;
	}

	fprintf(stderr,"Adding %lu, %lu\n\n", rel,col);

	indexWarehouse->rel[indexWarehouse->size] = rel;
	indexWarehouse->col[indexWarehouse->size] = col;
	indexWarehouse->hash1[indexWarehouse->size] = hash1;
	indexWarehouse->indexes[indexWarehouse->size] = indexes;
	(indexWarehouse->size)++;

	relationIndex *a= indexes->indexes;
	while (a)
	{
		curmem+=a->rel->size;
		a = a->next;

	}
	fprintf(stderr,"Current memory usage for the indexWarehouse is %lu\n\n", (myint_t) (curmem * sizeof(tuple) *1.5));
	
	return;
}

void AddToReWarehouse(myint_t rel, myint_t col, myint_t hash1, reorderedR *array)
{
	if (reWarehouse == NULL)
	{
		reWarehouse = malloc(sizeof(wares));
		reWarehouse ->size = 0;
		reWarehouse->rel = malloc(5000*sizeof(myint_t));
		reWarehouse->col = malloc(5000*sizeof(myint_t));
		reWarehouse->hash1 = malloc(5000*sizeof(myint_t));
		reWarehouse->arrays = malloc(5000*sizeof(reorderedR*));
		return;
	}

	fprintf(stderr,"Adding ror %lu, %lu\n\n", rel,col);

	reWarehouse->rel[reWarehouse->size] = rel;
	reWarehouse->col[reWarehouse->size] = col;
	reWarehouse->hash1[reWarehouse->size] = hash1;
	reWarehouse->arrays[reWarehouse->size] = array;
	(reWarehouse->size)++;
	
	return;
}

myint_t distValues(relation *r)
{
	if (r->size==0) return 0;
	tuple *col = r->tuples;
	myint_t rows = r->size;

	stats * statsStruct = malloc(sizeof(stats));
    statsStruct->minI = col[0].key;
    statsStruct->maxU = col[0].key;
    statsStruct->numElements = rows;
    statsStruct->distinctVals = 0;
       

    myint_t i;
    for( i = 0; i < rows; i++) {

        if (statsStruct->minI > col[i].key ) {
            statsStruct->minI = col[i].key;
        }

        else if ( statsStruct->maxU < col[i].key ) {
            statsStruct->maxU = col[i].key;
        }

    }


    if ( statsStruct->maxU - statsStruct->minI + 1 > N ) {
        
        myint_t size = (myint_t) ceil( (double)N / (double)(sizeof(myint_t) * 8) );
        statsStruct->distinctArray = (myint_t *) malloc( size * sizeof(myint_t) );

        for( i = 0; i < size; i ++)
            statsStruct->distinctArray[i] = 0;

        for( i = 0; i < rows/BETTERER; i++) {

            if ( TestBit(statsStruct->distinctArray, (col[i].key - statsStruct->minI) % N) == 0) {     //check if it is the first occurence of col[i] value and increment distinctVals
                statsStruct->distinctVals++;
            }
 
            SetBit(statsStruct->distinctArray, (col[i].key - statsStruct->minI) % N );             //set ((col[i] - statsStruct->minI) % N)-th bit to True
            
        }
        
    } else {

        myint_t size = (myint_t) ceil( (double) (statsStruct->maxU - statsStruct->minI + 1)  / (double)(sizeof(myint_t) * 8) );
        statsStruct->distinctArray = (myint_t *) malloc(size * sizeof(myint_t));
        

        for( i = 0; i < size; i ++)
            statsStruct->distinctArray[i] = 0;
        

        for( i = 0; i < rows/BETTERER; i++) {

            if ( TestBit(statsStruct->distinctArray, col[i].key - statsStruct->minI) == 0) {     //check if it is the first occurence of col[i] value and increment distinctVals
                
                statsStruct->distinctVals++;
            }
 
            SetBit(statsStruct->distinctArray, col[i].key - statsStruct->minI );             //set (col[i] - statsStruct->minI)-th bit to True
            
        }
    }

    free(statsStruct->distinctArray);
    myint_t dv = statsStruct->distinctVals;
    free(statsStruct);

    return dv*BETTERER;
}

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
	return 1/((double)sames*100);
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
	myint_t dvalues=distValues(r);
	//double identicality = IdenticalityTest(r);
	*hash1 = FindNextPower(floor(ERROR_MARGIN * (dvalues * sizeof(tuple) / AVAILABLE_CACHE_SIZE)) + 1); //ERROR_MARGIN ==1.05 to fit
	fprintf(stderr, "hash1 = %lu\n", *hash1);
	myint_t size = r->size, *hist, prevBad, max, beginning, maxBucketSize = floor(AVAILABLE_CACHE_SIZE / sizeof(tuple)), nextPower;
	hist = malloc(*hash1 * sizeof(myint_t));
	if (hist ==NULL)
    {
            perror("Wrong arguments");
            exit(1);
    }

	myint_t bad = DoTheHash(r,*hash1,hist,hash_values,&max,0);
	beginning = *hash1;
	prevBad = bad;

	/*while (bad > 0)
    {
		nextPower = (myint_t)log2(FindNextPower(max/maxBucketSize)+1);
		if (log2(*hash1) + nextPower > floor(log2(size)) || log2(*hash1) + nextPower > floor(log2(BUCKET_MEMORY_LIMIT)) || max <= 1.1 * dvalues)
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
    }*/

	return hist;
}

