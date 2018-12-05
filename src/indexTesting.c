#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <CUnit/Automated.h>
#include <CUnit/CUnit.h>


#include "sideFunctions.h"
#include "hashing.h"
#include "indexManip.h"
#include "CUnit/Basic.h"
#define NUM_OF_TUPLES 10
#define HASH1 3
#define HASH2 4
#define PSUMSIZE 3

myint_t * chain = NULL;
myint_t * buckets = NULL;

relationIndex * oneIndex;

reorderedR * ror = NULL;
indexArray * indexA = NULL;
relationIndex * newIndex = NULL;
tuple * startOfBucket = NULL;

int InitializeRor() {

	ror = (reorderedR *) malloc(sizeof(reorderedR));
	
	ror->pSumArr.psumSize = PSUMSIZE;
	ror->pSumArr.psum = (pSumTuple *) malloc(PSUMSIZE * sizeof(pSumTuple));
	int i;
	for(i = 0; i < PSUMSIZE; i++) {
		ror->pSumArr.psum[i].h1Res = i;
		ror->pSumArr.psum[i].offset = i * 5;
	}

	ror->rel = (relation *) malloc(sizeof(relation));
	ror->rel->size = 15;
	ror->rel->tuples = (tuple *) malloc(15 * sizeof(tuple));

	int j = 3;
	for( i = 0; i < 5; i++) {
		ror->rel->tuples[i].payload = j;
		ror->rel->tuples[i].key = j;
		j += 3; 
	}

	j = 1;
	for( i = 5; i < 10; i++) {
		ror->rel->tuples[i].payload = j;
		ror->rel->tuples[i].key = j;
		j += 3; 
	}	

	j = 2;
	for( i = 10; i < 15; i++) {
		ror->rel->tuples[i].payload = j;
		ror->rel->tuples[i].key = j;
		j += 3; 
	}	

	startOfBucket = (tuple *) malloc(NUM_OF_TUPLES * sizeof(tuple));
	for(i = 0; i < 10; i++) {

		startOfBucket[i].payload = i;
		startOfBucket[i].key = (i + 1) * 25;
	}


	return 0;

}

void RecFreeIndex(relationIndex * rI) {

	relationIndex * temp = rI;

	if(temp->next != NULL) 
		RecFreeIndex(temp->next);

	free(temp->chain);
	free(temp->buckets);
	free(temp->rel);
	free(temp);
	return;
}

int freeRor() {

	free(ror->rel->tuples);
	free(ror->rel);
	free(ror->pSumArr.psum);
	free(ror);
	free(startOfBucket);
	RecFreeIndex(newIndex);
	return 0;
}



void testStartOfBucket() {

	relation *rel = getStartOfBucket(ror, 1);

	CU_ASSERT(rel->tuples[0].key == 1);
	free(rel);
	rel = getStartOfBucket(ror, 2);
	CU_ASSERT(rel->tuples[3].key == 11);
	free(rel);

	rel = getStartOfBucket(ror, 0);
	CU_ASSERT(rel->tuples[1].key == 6);
	free(rel);

	return;
}

void testBuildSubIndex() {

	myint_t sizeAll = 10, eachSize = 3, key = 10, sizeIndexedSofar = 3;
	sizeAll -= eachSize;
	buildSubIndex(&newIndex, HASH1, HASH2, sizeAll, eachSize, sizeIndexedSofar, startOfBucket, key);
	
	relationIndex * temp = newIndex;

	CU_ASSERT(temp->buckets[12] == 1);
	CU_ASSERT(temp->buckets[15] == 2);
	CU_ASSERT(temp->buckets[2] == 3);

	for(int i = 0; i < eachSize; i++ )
		CU_ASSERT(temp->chain[i] == 0);

	temp = temp->next;

	CU_ASSERT(temp->buckets[5] == 1);
	CU_ASSERT(temp->buckets[9] == 2);
	CU_ASSERT(temp->buckets[12] == 3);
	for(int i = 0; i < eachSize; i++ )
		CU_ASSERT(temp->chain[i] == 0);

	temp = temp->next;

	CU_ASSERT(temp->buckets[15] == 1);
	
	for(int i = 0; i < 1; i++ )
		CU_ASSERT(temp->chain[i] == 0);

	return;
}


int InitializeIndexTest() {

	relation * rel = (relation *) malloc(sizeof(relation));
	rel->size = NUM_OF_TUPLES;
	rel->tuples = (tuple *) malloc(NUM_OF_TUPLES * sizeof(tuple));
	int i;
	for( i = 0; i < NUM_OF_TUPLES; i++){
		rel->tuples[i].payload = i;
		rel->tuples[i].key = pow(i, 2);
	}

	int bucketSize = hash2Range(HASH2);
	oneIndex = (relationIndex *) malloc(sizeof(relationIndex));
	*oneIndex = initializeIndex(bucketSize, rel, 6, NULL, HASH2);

	return 0;

}

int freeIndexTest() {

	free(oneIndex->chain);
	free(oneIndex->buckets);
	free(oneIndex->rel->tuples);
	free(oneIndex->rel);
	free(oneIndex);
	return 0;
}

void testBuildIndex(){


	buildIndex(oneIndex, HASH1, HASH2);

	int i;
	CU_ASSERT(oneIndex->chain[0]== 0);
	CU_ASSERT(oneIndex->chain[1]== 1);
	CU_ASSERT(oneIndex->chain[2]== 2);
	for(i = 3; i < 10; i++) {
		CU_ASSERT(oneIndex->chain[i] == 0);
	}
	
	
	CU_ASSERT(oneIndex->buckets[0] == 3);
	CU_ASSERT(oneIndex->buckets[1] == 4);
	CU_ASSERT(oneIndex->buckets[2] == 5);
	CU_ASSERT(oneIndex->buckets[3] == 6);
	CU_ASSERT(oneIndex->buckets[4] == 7);
	CU_ASSERT(oneIndex->buckets[5] == 0);
	CU_ASSERT(oneIndex->buckets[6] == 8);
	CU_ASSERT(oneIndex->buckets[7] == 0);
	CU_ASSERT(oneIndex->buckets[8] == 9);
	CU_ASSERT(oneIndex->buckets[9] == 0);
	CU_ASSERT(oneIndex->buckets[10] == 10);

	for(i = 11; i < 16; i++)
		CU_ASSERT(oneIndex->buckets[i] == 0);

	return;
}

int InitializeStructs() {

	chain = (myint_t *) malloc(8 * sizeof(myint_t));
	buckets = (myint_t *) malloc(5 * sizeof(myint_t));

	int i;
	for(i = 0; i < 8; i++)
		chain[i] = 0;
	for(i = 0; i < 5; i++)
		buckets[i] = 0;

	
	return 0;

}

int freeStructs() {

	free(chain);
	free(buckets);
	return 0;
}

void testUpdateChain() {

	
	updateChain(chain, buckets, 3, 2);
	updateChain(chain, buckets, 3, 3);
	updateChain(chain, buckets, 3, 6);

	CU_ASSERT(chain[2] == 0);
	CU_ASSERT(chain[3] == 3);
	CU_ASSERT(chain[6] == 4);
	CU_ASSERT(buckets[3] == 7);
	return;
}

void testHashing() {

	myint_t value = 9;
	value = hashing(value, HASH1, HASH2);
	CU_ASSERT(value == 1);
	value = 81;
	value = hashing(value, HASH1, HASH2);
	CU_ASSERT(value == 10);

	value = 64;
	value = hashing(value, HASH1, HASH2);
	CU_ASSERT(value == 8);

	value = 49;
	value = hashing(value, HASH1, HASH2);
	CU_ASSERT(value == 6);

	value = 666;
	value = hashing(value, HASH1, HASH2);
	CU_ASSERT(value == 2);
	
	value = 2147483;
	value = hashing(value, HASH1, HASH2);
	CU_ASSERT(value == 9);
	

	return;
}

int main(void) {

	CU_pSuite pSuite = NULL;
	CU_pSuite pSuite2 = NULL;
	CU_pSuite pSuite3 = NULL;
	CU_pSuite pSuite4 = NULL;

	//Initialize the CUnit test registry
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

  /* add a suite to the registry */
   pSuite = CU_add_suite("Chain Suite", InitializeStructs, freeStructs);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   // add the tests to the suite 
   if ((NULL == CU_add_test(pSuite, "Test updateChain", testUpdateChain)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add a 2nd suite to the registry */
   pSuite2 = CU_add_suite("BuildIndex Suite", InitializeIndexTest, freeIndexTest);
   if (NULL == pSuite2) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the 2nd suite */
   if ((NULL == CU_add_test(pSuite2, "Test buildIndex", testBuildIndex)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

    //add a 3rd suite to the registry 
   pSuite3 = CU_add_suite("Indexing Suite", InitializeRor, freeRor);
   if (NULL == pSuite3) {
      CU_cleanup_registry();
      return CU_get_error();
   }

    //add the tests to the 3rd suite 
   if ((NULL == CU_add_test(pSuite3, "Test getStartOfBucket", testStartOfBucket )) ||
   		(NULL == CU_add_test(pSuite3, "Test buildSubIndex", testBuildSubIndex )))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add a 3rd suite to the registry */
   pSuite4 = CU_add_suite("Hashing Suite", NULL, NULL);
   if (NULL == pSuite4) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the 3rd suite */
   if ((NULL == CU_add_test(pSuite4, "Test hashing", testHashing )) )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();

   CU_cleanup_registry();
   return CU_get_error();
}

