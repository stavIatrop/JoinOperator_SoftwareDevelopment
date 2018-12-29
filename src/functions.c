#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "basicStructs.h"
#include "viceFunctions.h"
#include "jobScheduler.h"

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
	
	myint_t size = r->size, limit = size/10, sames=0;
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

myint_t DoTheHash(relation *r, myint_t hash1, myint_t *hist, myint_t *hash_values, myint_t *max, char trivial)
{	//hash1 is the number of buckets in the hashing, hist is the histogram and hash_values the hash value for each key
	//myint_t maxBucketSize = floor(AVAILABLE_CACHE_SIZE / sizeof(tuple));
	myint_t i, size = r->size, value;
	for (i = 0; i < hash1; i++) hist[i] = 0;

        for (i = 0; i < size; i++)
        {
                value = Hash1_2(r->tuples[i].key,hash1);
                hash_values[i] = value;
                hist[value] = hist[value] + 1;
        }
    return 0;

	/*if (trivial!=0) return 0; //below here is only for calculating the best hash1. If we just want to do the hash, trivial!=0.

        for (i=0; i<hash1; i++)
        {
		value = hist[i];
		if (value > maxBucketSize)
		{
			bad++;
			*max += value;
		}
        }
	return bad;*/
}


myint_t *Hash1(relation *r,myint_t hash1, myint_t *hash_values)
{
	myint_t *hist = malloc(hash1 * sizeof(myint_t));
	if (hist ==NULL)
    {
            perror("Simon says: malloc failed");
            exit(1);
    }

	myint_t i, size = r->size, value;
	for (i = 0; i < hash1; i++) hist[i] = 0;

    for (i = 0; i < size; i++)
    {
            value = Hash1_2(r->tuples[i].key,hash1);
            hash_values[i] = value;
            hist[value] = hist[value] + 1;
    }
	return hist;
	/*double identicality=IdenticalityTest(r);
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

	return hist;*/
}
