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

/*SEARCH TESTS*/
int initialiseRelations() {
        rRel = malloc(sizeof(relation));
        sRel = malloc(sizeof(relation));

        
        rRel->tuples = malloc(sizeR * sizeof(tuple));
	rRel->size=sizeR;
        sRel->tuples = malloc(sizeS * sizeof(tuple));
	sRel->size=sizeS;

        //srand(time(NULL));
        return 0;

}

int freeRelations() {
        free(rRel->tuples);
        free(sRel->tuples);
        free(rRel);
        free(sRel);

        return 0;
}
void testZeroResults() {
        for (myint_t i=0; i<sizeR; i++)
        {
                //random = rand();
                rRel->tuples[i].key = 5;
                rRel->tuples[i].payload = i;
        }

        for (myint_t i=0; i<sizeS; i++)
        {
                //random = rand();
                sRel->tuples[i].key = 6;
                sRel->tuples[i].payload = i;
        }

        myint_t h1 = FIRST_REORDERED;
        RoR = reorderRelation(rRel, &h1);
        RoS = reorderRelation(sRel, &h1);

        indexA = indexing(RoR, h1);

        list = searchThreadVer(indexA, RoS);

        CU_ASSERT(list->numbOfNodes == 0);

        free(RoR->pSumArr.psum);
        free(RoR);
        free(RoS->pSumArr.psum);
        free(RoS);
        
        freeIndexArray(indexA);

        freeResultList(list);
}

void testOrderedResults() {
        for (myint_t i=0; i<sizeR; i++)
        {
                //random = rand();
                rRel->tuples[i].key = i;
                rRel->tuples[i].payload = i;
        }

        for (myint_t i=0; i<sizeS; i++)
        {
                //random = rand();
                sRel->tuples[i].key = i;
                sRel->tuples[i].payload = i;
        }

        myint_t h1 = FIRST_REORDERED;
        RoR = reorderRelation(rRel, &h1);
        RoS = reorderRelation(sRel, &h1);

        indexA = indexing(RoR, h1);

        list = search(indexA, RoS);

        CU_ASSERT(list->numbOfNodes == sizeR / tuplesPerNode + 1);
        resultNode * node = list->firstNode;
        int whichRes = 0;
        for(int i = 0; i < list->numbOfNodes; i++) {
                for(int j = 0; j < node->size; j++) {
                        CU_ASSERT(node->tuples[j].rowR == node->tuples[j].rowS);
                        CU_ASSERT(node->tuples[j].rowR == whichRes);
                        whichRes++;
                }
                node = node->nextNode;
        }

        free(RoR->pSumArr.psum);
        free(RoR);
        free(RoS->pSumArr.psum);
        free(RoS);
        
        freeIndexArray(indexA);

        freeResultList(list);
}

void testInversedOrderedResults() {
        for (myint_t i=0; i<sizeR; i++)
        {
                //random = rand();
                rRel->tuples[i].key = i;
                rRel->tuples[i].payload = i;
        }

        for (myint_t i=0; i<sizeS; i++)
        {
                //random = rand();
                sRel->tuples[i].key = sizeR - i;
                sRel->tuples[i].payload = i;
        }

        myint_t h1 = FIRST_REORDERED;
        RoR = reorderRelation(rRel, &h1);
        RoS = reorderRelation(sRel, &h1);

        indexA = indexing(RoR, h1);

        list = search(indexA, RoS);

        CU_ASSERT(list->numbOfNodes == sizeR / tuplesPerNode + 1);
        resultNode * node = list->firstNode;
        for(int i = 0; i < list->numbOfNodes; i++) {
                for(int j = 0; j < node->size; j++) {
                        CU_ASSERT(node->tuples[j].rowR == sizeR - node->tuples[j].rowS);
                }
                node = node->nextNode;
        }

        free(RoR->pSumArr.psum);
        free(RoR);
        free(RoS->pSumArr.psum);
        free(RoS);
        
        freeIndexArray(indexA);

        freeResultList(list);
}

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

	// CU_pSuite pSuite = NULL;
	// CU_pSuite pSuite2 = NULL;
	// CU_pSuite pSuite3 = NULL;
	CU_pSuite pSuite4 = NULL;
   	CU_pSuite pSuite5 = NULL;


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
   pSuite5 = CU_add_suite("Search Suite", initialiseRelations, freeRelations);
   if (NULL == pSuite5) {
      CU_cleanup_registry();
      return CU_get_error();
   }

    /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite5, "Test No Matching Values", testZeroResults)) ||
       (NULL == CU_add_test(pSuite5, "Test All Matching Values Ordered", testOrderedResults)) ||
       (NULL == CU_add_test(pSuite5, "Test All Matching Values Inversed Ordered", testInversedOrderedResults)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }
   // CU_set_output_filename("Chain_Test");
   // CU_automated_run_tests();
   // CU_list_tests_to_file();

   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();

   unsigned int returnValue  = CU_get_number_of_tests_failed();

   CU_cleanup_registry();

   return returnValue;

}