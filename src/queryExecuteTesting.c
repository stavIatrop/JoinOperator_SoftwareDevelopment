#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/Automated.h>
#include <CUnit/CUnit.h>
#include "CUnit/Basic.h"

#include "queryStructs.h"
#include "interListInterface.h"

headInter * head = NULL;

//Test intermediate list accessing, adding, deleting
int initList() {
    head = initialiseHead();

    return 0;
}

int freeList() {
    freeInterList(head);

    return 0;
} 

myint_t ** create_2DArray(int cols, int rows) {
    myint_t ** arr = (myint_t **) malloc(cols * sizeof(myint_t *));
    for(int i = 0; i < cols; i++) {
        arr[i] = (myint_t *) malloc(rows * sizeof(myint_t));
    }
    for(int whichCol = 0; whichCol < cols; whichCol++) {
        for(int whichRow = 0; whichRow < rows; whichRow++) {
            arr[whichCol][whichRow] = 1;
        }
    }
    return arr;
}

myint_t * create_1DArray(int rows, int value) {
    myint_t * arr = (myint_t *) malloc(rows * sizeof(myint_t));
    for(int whichRow = 0; whichRow < rows; whichRow++) {
        arr[whichRow] = value;
    }
    return arr;
}


void testPushInter() {
    myint_t ** rowIds = create_2DArray(3, 20);
    myint_t * rels = create_1DArray(3, 1);
    
    pushInter(head, 3, 20, rels, rowIds);

    CU_ASSERT(head->numOfIntermediates == 1);
    CU_ASSERT(head->start->data->rowIds[0][0] == 1);
    CU_ASSERT(head->start->data->rowIds[0][10] == 1);
    CU_ASSERT(head->start->data->rowIds[2][5] == 1);
    CU_ASSERT(head->start->data->joinedRels[0] == 1);
    CU_ASSERT(head->start->data->numOfCols == 3);
    CU_ASSERT_PTR_NULL(head->start->next);

    rowIds = create_2DArray(3, 20);
    rels = create_1DArray(3, 2);
    pushInter(head, 3, 20, rels, rowIds);

    CU_ASSERT(head->numOfIntermediates == 2);
    CU_ASSERT(head->start->next->data->rowIds[0][0] == 1);
    CU_ASSERT(head->start->next->data->rowIds[0][0] == 1);
    CU_ASSERT(head->start->next->data->joinedRels[0] == 2);
    CU_ASSERT_PTR_NULL(head->start->next->next);
}

void testUpdatehInter() {
    myint_t ** rowIds = create_2DArray(3, 20);
    myint_t * rels = create_1DArray(3, 3);
    updateInter(head->start, 3, 20, rels, rowIds);
    CU_ASSERT(head->numOfIntermediates == 2);
    CU_ASSERT(head->start->data->rowIds[0][0] == 1);
    CU_ASSERT(head->start->data->joinedRels[0] == 3);
    CU_ASSERT(head->start->data->numOfCols == 3);
}

void testDeleteNode() {

    deleteInterNode(head, head->start);
    CU_ASSERT(head->numOfIntermediates == 1);
    CU_ASSERT(head->start->data->joinedRels[0] == 2)
    CU_ASSERT_PTR_NULL(head->start->next);

    myint_t * rels = create_1DArray(3, 4);
    myint_t ** rowIds = create_2DArray(3, 20);
    pushInter(head, 3, 20, rels, rowIds);

    rowIds = create_2DArray(3, 20);
    rels = create_1DArray(3, 5);
    pushInter(head, 3, 20, rels, rowIds);
    CU_ASSERT(head->numOfIntermediates == 3);
    deleteInterNode(head, head->start->next);
    CU_ASSERT(head->numOfIntermediates == 2);

    CU_ASSERT(head->start->next->data->joinedRels[0] == 5);

    CU_ASSERT_PTR_NULL(head->start->next->next);

}


int main(void) {

	CU_pSuite pSuite1 = NULL;
	


	//Initialize the CUnit test registry
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

  /* add a suite to the registry */
   pSuite1 = CU_add_suite("Test Intermediate List", initList, freeList);
   if (NULL == pSuite1) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite1, "Test Pushing", testPushInter)) ||
       (NULL == CU_add_test(pSuite1, "Test Updating", testUpdatehInter)) ||
       (NULL == CU_add_test(pSuite1, "Test Deleting", testDeleteNode)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   

   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();

   CU_cleanup_registry();
   
   unsigned int returnValue  = CU_get_number_of_tests_failed();

   CU_cleanup_registry();

   return returnValue;
}
