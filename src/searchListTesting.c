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

myint_t * chain = NULL;
myint_t * buckets = NULL;

relationIndex oneIndex;

reorderedR * ror = NULL;
indexArray * indexA = NULL;

headResult * list = NULL;
relationIndex * rIndex = NULL;
int tuplesPerNode = MB / sizeof(tuple);
reorderedR * RoR = NULL, * RoS = NULL;
int sizeR = 1000, sizeS = 1000;
relation * rRel, * sRel;


/*RESULT LIST TESTS*/
int createList(void) {
        list = initialiseResultHead();

        for(int i = 0; i < TUPLE_NUMB; i++) {
                rowTuple temp;
                temp.rowR = (myint_t) i;
                temp.rowS = (myint_t) i;

                pushResultVer2(list, &temp);
        }
        cleanListHead(list);
        return 0;
}

int freeList(void) {
        freeResultList(list);
        return 0;
}

void testNumbOfNodes(void) {
        
        unsigned long int targetNodes = (TUPLE_NUMB * sizeof(tuple)) / MB;
        printf("EXPECTED = %ld | RESULT = %ld\n", targetNodes, list->numbOfNodes);
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


int main(void) {

	CU_pSuite pSuite4 = NULL;
   	//CU_pSuite pSuite5 = NULL;


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


   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();

   unsigned int returnValue  = CU_get_number_of_tests_failed();

   CU_cleanup_registry();

   return returnValue;

}