#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <CUnit/Automated.h>
#include <CUnit/CUnit.h>


#include "sideFunctions.h"
#include "hashing.h"
#include "indexManip.h"
#include "CUnit/Basic.h"
#define NUM_OF_TUPLES 15
#define HASH2 5

uint32_t * chain = NULL;
uint32_t * buckets = NULL;
relationIndex oneIndex;

int InitializeIndexTest() {

	relation * rel = (relation *) malloc(sizeof(relation));
	rel->size = NUM_OF_TUPLES;
	rel->tuples = (tuple *) malloc(NUM_OF_TUPLES * sizeof(tuple));
	int i;
	for( i = 0; i < NUM_OF_TUPLES; i++){
		rel->tuples[i].key = i;
		rel->tuples[i].payload = i + 1;
	}

	int bucketSize = hash2Range(HASH2);
	oneIndex = initializeIndex(bucketSize, rel, 6);
	return 0;

}

int freeIndexTest() {

	free(oneIndex.chain);
	free(oneIndex.buckets);
	free(oneIndex.rel);
	return 0;
}

void testBuildIndex(){


	buildIndex(&oneIndex, HASH2);

	int i;

	for(i = 0; i < 5; i++)
		CU_ASSERT(oneIndex.chain[i] == 0);

	for(i = 1; i <= 10; i++)
		CU_ASSERT(oneIndex.chain[i + 4] == i);	
	
	CU_ASSERT(oneIndex.buckets[0] == 15);
	CU_ASSERT(oneIndex.buckets[1] == 11);
	CU_ASSERT(oneIndex.buckets[2] == 12);
	CU_ASSERT(oneIndex.buckets[3] == 13);
	CU_ASSERT(oneIndex.buckets[4] == 14);

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


int main(void) {

	CU_pSuite pSuite = NULL;
	CU_pSuite pSuite2 = NULL;

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

   // CU_set_output_filename("Chain_Test");
   // CU_automated_run_tests();
   // CU_list_tests_to_file();

   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();

   CU_cleanup_registry();
   return CU_get_error();

}