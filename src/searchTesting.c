#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "basicStructs.h"
#include "resultListInterface.h"
#include "chainFollowerInterface.h"

#include <CUnit/Automated.h>
#include "CUnit/Basic.h"

#define TUPLE_NUMB 10000000

headResult * list = NULL;
relationIndex * rIndex = NULL;
int tuplesPerNode = MB / sizeof(tuple);

/*RESULT LIST TESTS*/
int createList(void) {
        list = initialiseResultHead();

        for(int i = 0; i < TUPLE_NUMB; i++) {
                tuple temp;
                temp.key = (int32_t) i;
                temp.payload = (int32_t) i;

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
        CU_ASSERT(temp->tuples[testV1 % tuplesPerNode].key == testV1);

        int testV2 = 10000;
        temp = list->firstNode;
        for(int i = 0; i < testV2 / tuplesPerNode; i++) {
                temp = temp->nextNode;
        }
        CU_ASSERT(temp->tuples[testV2 % tuplesPerNode].key == testV2);

        int testV3 = 500000;
        temp = list->firstNode;
        for(int i = 0; i < testV3 / tuplesPerNode; i++) {
                temp = temp->nextNode;
        }
        CU_ASSERT(temp->tuples[testV3 % tuplesPerNode].key == testV3);

        int testV4 = TUPLE_NUMB - 1;
        temp = list->firstNode;
        for(int i = 0; i < testV4 / tuplesPerNode; i++) {
                temp = temp->nextNode;
        }
        CU_ASSERT(temp->tuples[testV4 % tuplesPerNode].key == testV4);
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
                rIndex->rel->tuples[i].key = i;
                rIndex->rel->tuples[i].payload = 12;
        }

        for(int i = 0; i < 10; i++) {
                rIndex->buckets[i] = 0;
        }
        rIndex->rel->tuples[2].payload = 2;

        //CARE: The above and below structs do not follow the rules
        rIndex->buckets[2] = 8;
        rIndex->chain[8] = 5;
        rIndex->chain[5] = 4;
        rIndex->chain[4] = 3;
        rIndex->chain[3] = 0;

        return 0;
}

int freeIndex(void) {
        free(rIndex->rel->tuples);
        free(rIndex->rel);
        free(rIndex->chain);
        free(rIndex->buckets);
        free(rIndex);

        return 0;
}

void testChainFollowNonExistent(void) {
        tuple t;
        t.key = 5;
        t.payload = 13;

        followChain(list, rIndex, &t, 10);

        CU_ASSERT(list->numbOfNodes == 0);
}

void testChainFollowExists(void) {
        tuple t;
        t.key = 5;
        t.payload = 12;

        followChain(list, rIndex, &t, 10);

        CU_ASSERT(list->numbOfNodes == 1);
        CU_ASSERT(list->firstNode->size == 3);
        CU_ASSERT(list->firstNode->tuples[0].key == 7);
        CU_ASSERT(list->firstNode->tuples[1].key == 4);
        CU_ASSERT(list->firstNode->tuples[2].key == 3);
}



int main()
{
   CU_pSuite pSuite = NULL;
   CU_pSuite pSuite2 = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Result List Suite", createList, freeList);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "Test Number of Nodes", testNumbOfNodes)) ||
       (NULL == CU_add_test(pSuite, "Test Specific Values", testValues)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

/* add a suite to the registry */
   pSuite2 = CU_add_suite("Chain Follower Suite", initialiseChainFollower, freeIndex);
   if (NULL == pSuite2) {
      CU_cleanup_registry();
      return CU_get_error();
   }

    /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite2, "Test Non Existing Value", testChainFollowNonExistent)) ||
       (NULL == CU_add_test(pSuite2, "Test Existing Value", testChainFollowExists)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   
   /*CU_set_output_filename("Search_Test");
   CU_automated_run_tests();
   CU_list_tests_to_file();*/

   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();

   CU_cleanup_registry();
   return CU_get_error();
}