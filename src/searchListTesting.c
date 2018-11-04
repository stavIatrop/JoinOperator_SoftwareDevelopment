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
int tuplesPerNode = MB / sizeof(tuple);


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
// int initialiseChainFollower() {
//         list = initialiseResultHead();
//         rIndex = (relationIndex *) malloc(sizeof(relationIndex));
//         rIndex->chain = (uint32_t *) malloc(20 * sizeof(uint32_t));
//         rIndex->buckets = (uint32_t *) malloc(10 * sizeof(uint32_t));
//         rIndex->rel = (relation *) malloc(sizeof(relation));
//         rIndex->rel->tuples = (tuple *) malloc(20 * sizeof(tuple));
//         rIndex->rel->size = 20;

//         for(int i = 0; i < rIndex->rel->size; i++) {
//                 rIndex->rel->tuples[i].payload = i;
//                 rIndex->rel->tuples[i].key = 12;
//         }

//         for(int i = 0; i < 10; i++) {
//                 rIndex->buckets[i] = 0;
//         }
//         rIndex->rel->tuples[2].key = 2;

//         //CARE: The above and below structs do not follow the rules
//         rIndex->hash2 = 10;
//         rIndex->next = NULL;
//         rIndex->buckets[2] = 8;
//         rIndex->chain[7] = 5;
//         rIndex->chain[4] = 4;
//         rIndex->chain[3] = 3;
//         rIndex->chain[2] = 0;

//         return 0;
// }

// int freeIndex(void) {
//         free(rIndex->rel->tuples);
//         free(rIndex->rel);
//         free(rIndex->chain);
//         free(rIndex->buckets);
//         free(rIndex);

//         freeResultList(list);

//         return 0;
// }

// void testChainFollowNonExistent(void) {
//         tuple t;
//         t.key = 13;
//         t.payload = 5;
        
//         followChain(list, rIndex, t);
//         CU_ASSERT(list->numbOfNodes == 0);
// }

// void testChainFollowExists(void) {
//         tuple t;
//         t.key = 12;
//         t.payload = 5;

//         followChain(list, rIndex, t);

//         CU_ASSERT(list->numbOfNodes == 1);

//         CU_ASSERT(list->firstNode->size == 3);
//         CU_ASSERT(list->firstNode->tuples[0].rowR == 7);
//         CU_ASSERT(list->firstNode->tuples[1].rowR == 4);
//         CU_ASSERT(list->firstNode->tuples[2].rowR == 3);
// }


int main(void) {

	// CU_pSuite pSuite = NULL;
	// CU_pSuite pSuite2 = NULL;
	// CU_pSuite pSuite3 = NULL;
	CU_pSuite pSuite4 = NULL;
   	// CU_pSuite pSuite5 = NULL;


	//Initialize the CUnit test registry
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

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
//    pSuite5 = CU_add_suite("Chain Follower Suite", initialiseChainFollower, freeIndex);
//    if (NULL == pSuite5) {
//       CU_cleanup_registry();
//       return CU_get_error();
//    }

//     /* add the tests to the suite */
//    if ((NULL == CU_add_test(pSuite5, "Test Non Existing Value", testChainFollowNonExistent)) ||
//        (NULL == CU_add_test(pSuite5, "Test Existing Value", testChainFollowExists)))
//    {
//       CU_cleanup_registry();
//       return CU_get_error();
//    }
   // CU_set_output_filename("Chain_Test");
   // CU_automated_run_tests();
   // CU_list_tests_to_file();

   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();

   unsigned int returnValue  = CU_get_number_of_tests_failed();

   CU_cleanup_registry();

   return returnValue;

}