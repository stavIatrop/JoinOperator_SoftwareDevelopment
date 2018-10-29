#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <CUnit/Automated.h>
#include <CUnit/CUnit.h>

#include "basicStructs.h"
#include "resultListInterface.h"
#include "chainFollowerInterface.h"
#include "sideFunctions.h"
#include "hashing.h"
#include "indexManip.h"
#include "CUnit/Basic.h"

#define NUM_OF_TUPLES 15
#define HASH1 3
#define HASH2 5
#define PSUMSIZE 3
#define TUPLE_NUMB 10000000

uint32_t * chain = NULL;
uint32_t * buckets = NULL;

relationIndex oneIndex;

reorderedR * ror = NULL;
indexArray * indexA = NULL;

headResult * list = NULL;
relationIndex * rIndex = NULL;
int tuplesPerNode = MB / sizeof(tuple) + 1;

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
	ror->rel->size = NUM_OF_TUPLES;
	ror->rel->tuples = (tuple *) malloc(NUM_OF_TUPLES * sizeof(tuple));

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

void testIndexing() {


	indexA = indexing(ror, HASH1, HASH2);

	CU_ASSERT(indexA->indexes[1].buckets[0] == 4);
	CU_ASSERT(indexA->indexes[1].buckets[1] == 1);
	CU_ASSERT(indexA->indexes[1].buckets[2] == 3);
	CU_ASSERT(indexA->indexes[1].buckets[3] == 5);
	CU_ASSERT(indexA->indexes[1].buckets[4] == 2);
	return;
}

int InitializeIndexTest() {

	relation * rel = (relation *) malloc(sizeof(relation));
	rel->size = NUM_OF_TUPLES;
	rel->tuples = (tuple *) malloc(NUM_OF_TUPLES * sizeof(tuple));
	int i;
	for( i = 0; i < NUM_OF_TUPLES; i++){
		rel->tuples[i].payload = i;
		rel->tuples[i].key = i + 1;
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

/*RESULT LIST TESTS*/
int createList(void) {
        list = initialiseResultHead();

        for(int i = 0; i < TUPLE_NUMB; i++) {
                rowTuple temp;
                temp.rowR = (int32_t) i;
                temp.rowS = (uint32_t) i;

                pushResult(list, &temp);
        }
        return 0;
}

int freeList(void) {
        freeResultList(list);
        return 0;
}

void testNumbOfNodes(void) {
        
        unsigned long int targetNodes = (TUPLE_NUMB * sizeof(tuple)) / MB + 1;
        CU_ASSERT(targetNodes == list->numbOfNodes);
}

void testValues(void) {
        int testV1 = 0;
        resultNode * temp = list->firstNode;
        for(int i = 0; i < testV1 / tuplesPerNode; i++) {
                temp = temp->nextNode;
        }
        CU_ASSERT(temp->tuples[testV1 % tuplesPerNode].rowR == testV1);

        int testV2 = 10000;
        temp = list->firstNode;
        for(int i = 0; i < testV2 / tuplesPerNode; i++) {
                temp = temp->nextNode;
        }
        CU_ASSERT(temp->tuples[testV2 % tuplesPerNode].rowR == testV2);

        int testV3 = 500000;
        temp = list->firstNode;
        for(int i = 0; i < testV3 / tuplesPerNode; i++) {
                temp = temp->nextNode;
        }
        CU_ASSERT(temp->tuples[testV3 % tuplesPerNode].rowR == testV3);

        int testV4 = TUPLE_NUMB - 1;
        temp = list->firstNode;
        for(int i = 0; i < testV4 / tuplesPerNode; i++) {
                temp = temp->nextNode;
        }
        CU_ASSERT(temp->tuples[testV4 % tuplesPerNode].rowR == testV4);
}

/*CHAIN FOLLOWER TESTS*/
int initialiseChainFollower() {
        list = initialiseResultHead();
        rIndex = (relationIndex *) malloc(sizeof(relationIndex));
        rIndex->chain = (uint32_t *) malloc(20 * sizeof(uint32_t));
        rIndex->buckets = (uint32_t *) malloc(10 * sizeof(uint32_t));
        rIndex->rel = (relation *) malloc(sizeof(relation));
        rIndex->rel->tuples = (tuple *) malloc(20 * sizeof(tuple));
        rIndex->rel->size = 20;

        for(int i = 0; i < rIndex->rel->size; i++) {
                rIndex->rel->tuples[i].payload = i;
                rIndex->rel->tuples[i].key = 12;
        }

        for(int i = 0; i < 10; i++) {
                rIndex->buckets[i] = 0;
        }
        rIndex->rel->tuples[2].key = 2;

        //CARE: The above and below structs do not follow the rules
        rIndex->buckets[2] = 8;
        rIndex->chain[7] = 5;
        rIndex->chain[4] = 4;
        rIndex->chain[3] = 3;
        rIndex->chain[2] = 0;

        return 0;
}

int freeIndex(void) {
        free(rIndex->rel->tuples);
        free(rIndex->rel);
        free(rIndex->chain);
        free(rIndex->buckets);
        free(rIndex);

        freeResultList(list);

        return 0;
}

void testChainFollowNonExistent(void) {
        tuple t;
        t.key = 13;
        t.payload = 5;
        
        followChain(list, rIndex, t, 10);
        CU_ASSERT(list->numbOfNodes == 0);
}

void testChainFollowExists(void) {
        tuple t;
        t.key = 12;
        t.payload = 5;

        followChain(list, rIndex, t, 10);

        CU_ASSERT(list->numbOfNodes == 1);

        CU_ASSERT(list->firstNode->size == 3);
        CU_ASSERT(list->firstNode->tuples[0].rowR == 7);
        CU_ASSERT(list->firstNode->tuples[1].rowR == 4);
        CU_ASSERT(list->firstNode->tuples[2].rowR == 3);
}


int main(void) {

	CU_pSuite pSuite = NULL;
	CU_pSuite pSuite2 = NULL;
	CU_pSuite pSuite3 = NULL;
	CU_pSuite pSuite4 = NULL;
   	CU_pSuite pSuite5 = NULL;


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

/* add a suite to the registry */
   pSuite4 = CU_add_suite("Result List Suite", createList, freeList);
   if (NULL == pSuite4) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite4, "Test Number of Nodes", testNumbOfNodes)) ||
       (NULL == CU_add_test(pSuite4, "Test Specific Values", testValues)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

/* add a suite to the registry */
   pSuite5 = CU_add_suite("Chain Follower Suite", initialiseChainFollower, freeIndex);
   if (NULL == pSuite5) {
      CU_cleanup_registry();
      return CU_get_error();
   }

    /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite5, "Test Non Existing Value", testChainFollowNonExistent)) ||
       (NULL == CU_add_test(pSuite5, "Test Existing Value", testChainFollowExists)))
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