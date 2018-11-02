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

uint32_t * chain = NULL;
uint32_t * buckets = NULL;

relationIndex oneIndex;

reorderedR * ror = NULL;
indexArray * indexA = NULL;

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


	return 0;

}

int freeRor() {

	free(ror->rel->tuples);
	free(ror->rel);
	free(ror->pSumArr.psum);
	free(ror);
	free(indexA);
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

void testIndexing() {		//for CACHE_SIZE = 120


	indexA = indexing(ror, HASH1, HASH2);


	int i;
	for(i = 0; i < 3; i++) {
		CU_ASSERT(indexA->indexes[1].chain[i] == i);
	}
	CU_ASSERT(indexA->indexes[1].chain[3] == 0);
	CU_ASSERT(indexA->indexes[1].chain[4] == 4);

	for(i = 2; i < hash2Range(indexA->indexes[1].hash2); i++)
		CU_ASSERT(indexA->indexes[1].buckets[i] == 0);
	CU_ASSERT(indexA->indexes[1].buckets[0] == 3);
	CU_ASSERT(indexA->indexes[1].buckets[1] == 5);
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
	oneIndex = initializeIndex(bucketSize, rel, 6, NULL, HASH2);
	return 0;

}

int freeIndexTest() {

	free(oneIndex.chain);
	free(oneIndex.buckets);
	free(oneIndex.rel);
	return 0;
}

void testBuildIndex(){


	buildIndex(&oneIndex, HASH1, HASH2);

	int i;
	CU_ASSERT(oneIndex.chain[0]== 0);
	CU_ASSERT(oneIndex.chain[1]== 1);
	CU_ASSERT(oneIndex.chain[2]== 2);
	for(i = 3; i < 10; i++) {
		CU_ASSERT(oneIndex.chain[i] == 0);
	}
	
	
	CU_ASSERT(oneIndex.buckets[0] == 3);
	CU_ASSERT(oneIndex.buckets[1] == 4);
	CU_ASSERT(oneIndex.buckets[2] == 5);
	CU_ASSERT(oneIndex.buckets[3] == 6);
	CU_ASSERT(oneIndex.buckets[4] == 7);
	CU_ASSERT(oneIndex.buckets[5] == 0);
	CU_ASSERT(oneIndex.buckets[6] == 8);
	CU_ASSERT(oneIndex.buckets[7] == 0);
	CU_ASSERT(oneIndex.buckets[8] == 9);
	CU_ASSERT(oneIndex.buckets[9] == 0);
	CU_ASSERT(oneIndex.buckets[10] == 10);

	for(i = 11; i < 16; i++)
		CU_ASSERT(oneIndex.buckets[i] == 0);

	return;
}

int InitializeStructs() {

	chain = (uint32_t *) malloc(8 * sizeof(uint32_t));
	buckets = (uint32_t *) malloc(5 * sizeof(uint32_t));

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

	uint32_t value = 9;
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

   /* add the tests to the suite */
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

   /* add a 3rd suite to the registry */
   pSuite3 = CU_add_suite("Indexing Suite", InitializeRor, freeRor);
   if (NULL == pSuite3) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the 3rd suite */
   if ((NULL == CU_add_test(pSuite3, "Test getStartOfBucket", testStartOfBucket )) ||
   		(NULL == CU_add_test(pSuite3, "Test indexing", testIndexing )))
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

   // CU_set_output_filename("Chain_Test");
   // CU_automated_run_tests();
   // CU_list_tests_to_file();

   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();

   CU_cleanup_registry();
   return CU_get_error();
}
