#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/Automated.h>
#include <CUnit/CUnit.h>
#include "CUnit/Basic.h"

#include "queryStructs.h"
#include "interListInterface.h"
#include "basicStructs.h"
#include "resultListInterface.h"

#define TUPLE_NUMB 1000000

headInter * head = NULL;
headResult * headRes = NULL;
headInter * headInt = NULL;

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
    myint_t ** arr = (myint_t **) malloc(rows * sizeof(myint_t *));
    for(int i = 0; i < rows; i++) {
        arr[i] = (myint_t *) malloc(cols * sizeof(myint_t));
    }
    for(int whichRow = 0; whichRow < rows; whichRow++) {
        for(int whichCol = 0; whichCol < cols; whichCol++) {
            arr[whichRow][whichCol] = 1;
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
    CU_ASSERT(head->start->data->rowIds[10][0] == 1);
    CU_ASSERT(head->start->data->rowIds[5][2] == 1);
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

//Test Intermediate and Result interaction
int initIntermediatesResults() {

    //Initialise results
    headRes = initialiseResultHead();

    for(int i = 0; i < TUPLE_NUMB; i++) {
            rowTuple temp;
            temp.rowR = (int32_t) i;
            temp.rowS = (uint32_t) i;

            pushResult(headRes, &temp);
    }

    headInt = initialiseHead();

    return 0;
}

int freeIntermedieatesResults() {
    freeResultList(headRes);
    freeInterList(headInt);
    return 0;
}

void createResArrayTest() {
    myint_t size = 0;
    myint_t ** arr = createResultArray(headRes, &size);

    CU_ASSERT(size == TUPLE_NUMB);
    CU_ASSERT(arr[0][0] == 0);
    CU_ASSERT(arr[0][1] == 0);
    CU_ASSERT(arr[10000][0] == 10000);
    CU_ASSERT(arr[10000][1] == 10000);
    CU_ASSERT(arr[TUPLE_NUMB - 1][0] == TUPLE_NUMB - 1);
    CU_ASSERT(arr[TUPLE_NUMB - 1][1] == TUPLE_NUMB - 1);

    for(myint_t i = 0; i < TUPLE_NUMB; i++) {
        free(arr[i]);
    }
    free(arr);
}

void updateRowIdsTest() {
    headResult * headResLocal = initialiseResultHead();
    int rows = 1000;
    int cols = 3;
    myint_t ** rowIds = (myint_t **) malloc(rows * sizeof(myint_t *));
    for(int i = 0; i < rows; i++) {
        rowIds[i] = (myint_t *) malloc(cols * sizeof(myint_t));
    }
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            rowIds[i][j] = i;
        }
    }

    myint_t * joinedRels = (myint_t *) malloc(cols * sizeof(myint_t));
    for(int i = 0; i < cols; i++) {
        joinedRels[i] = i;
    }
    nodeInter * intNode = initialiseNode(cols, rows, joinedRels, rowIds);

    

    myint_t ** arr = updateRowIds(intNode, headResLocal, 0);
    CU_ASSERT_PTR_NULL(arr);

    for(int i = 0; i < rows / 2; i++) {
        rowTuple temp;
        temp.rowR = (int32_t) i * 2;
        temp.rowS = (uint32_t) i * 2;

        pushResult(headResLocal, &temp);
    }

    arr = updateRowIds(intNode, headResLocal, rows / 2);

    CU_ASSERT(arr[0][0] == 0);
    CU_ASSERT(arr[0][1] == 0);
    CU_ASSERT(arr[0][2] == 0);
    CU_ASSERT(arr[0][3] == 0);
    CU_ASSERT(arr[rows / 4][0] == rows / 2);
    CU_ASSERT(arr[rows / 4][3] == rows / 2);
    CU_ASSERT(arr[rows / 2 - 1][1] == rows - 2);
    CU_ASSERT(arr[rows / 2 - 1][2] == rows - 2);
    CU_ASSERT(arr[rows / 2 - 1][3] == rows - 2);

    for(int i = 0; i < rows / 2; i++) {
        free(arr[i]);
    }
    free(arr);
    freeResultList(headResLocal);
    freeNode(intNode);
}

void joinRowIdsTest() {
    headResult * headResLocal = initialiseResultHead();
    int rows = 1000;
    int cols = 2;
    myint_t ** rowIds = (myint_t **) malloc(rows * sizeof(myint_t *));
    for(int i = 0; i < rows; i++) {
        rowIds[i] = (myint_t *) malloc(cols * sizeof(myint_t));
    }
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            rowIds[i][j] = i;
        }
    }

    myint_t * joinedRels = (myint_t *) malloc(cols * sizeof(myint_t));
    for(int i = 0; i < cols; i++) {
        joinedRels[i] = i;
    }

    nodeInter * node1 = initialiseNode(cols, rows, joinedRels, rowIds);

    rowIds = (myint_t **) malloc(rows * sizeof(myint_t *));
    for(int i = 0; i < rows; i++) {
        rowIds[i] = (myint_t *) malloc(cols * sizeof(myint_t));
    }
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            rowIds[i][j] = i;
        }
    }

    joinedRels = (myint_t *) malloc(cols * sizeof(myint_t));
    for(int i = 0; i < cols; i++) {
        joinedRels[i] = i + cols;
    }
    
    nodeInter * node2 = initialiseNode(cols, rows, joinedRels, rowIds);

    for(int i = 0; i < rows / 2; i++) {
        rowTuple temp;
        temp.rowR = (int32_t) i * 2;
        temp.rowS = (uint32_t) i * 2;

        pushResult(headResLocal, &temp);
    }

    myint_t ** arr = joinRowIds(node1, node2, headResLocal, rows / 2);

    CU_ASSERT(arr[0][0] == 0);
    CU_ASSERT(arr[0][2] == 0);
    CU_ASSERT(arr[0][3] == 0);
    CU_ASSERT(arr[10][0] == 10 * 2);
    CU_ASSERT(arr[10][3] == 10 * 2);
    CU_ASSERT(arr[rows / 4][0] == rows /2);
    CU_ASSERT(arr[rows / 4][2] == rows / 2);
    CU_ASSERT(arr[rows / 2 - 1][1] == rows - 2);
    CU_ASSERT(arr[rows / 2 - 1][2] == rows - 2);
    CU_ASSERT(arr[rows / 2 - 1][3] == rows - 2);

    for(int i = 0; i < rows / 2; i++) {
        free(arr[i]);
    }
    free(arr);
    freeResultList(headResLocal);
    freeNode(node1);
    freeNode(node2);
}

void createInterSelfJoinTest() {
    myint_t rows = 100;
    myint_t * arr = (myint_t *) malloc(rows * sizeof(myint_t));
    for(int i = 0; i < rows; i++) {
        arr[i] = i;
    }

    createInterSelfJoin(headInt, 2, arr, rows);

    CU_ASSERT(headInt->start->data->joinedRels[0] == 2);
    CU_ASSERT(headInt->start->data->numbOfRows == rows);
    CU_ASSERT(headInt->start->data->numOfCols == 1);
    CU_ASSERT(headInt->start->data->rowIds[0][0] == 0);
    CU_ASSERT(headInt->start->data->rowIds[rows / 2][0] == rows / 2);
    CU_ASSERT(headInt->start->data->rowIds[rows - 1][0] == rows - 1);

    freeInterList(headInt);
    free(arr);
    headInt = initialiseHead();

}

int main(void) {

	CU_pSuite pSuite1 = NULL;
	CU_pSuite pSuite2 = NULL;



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

     /* add a suite to the registry */
   pSuite2 = CU_add_suite("Test Intermediate and Results Operations", initIntermediatesResults, freeIntermedieatesResults);
   if (NULL == pSuite2) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite2, "Test Create Results Array", createResArrayTest)) ||
       (NULL == CU_add_test(pSuite2, "Test Update Intermediate with Results List", updateRowIdsTest)) ||
       (NULL == CU_add_test(pSuite2, "Test Update 2 Intermediates with Results List", joinRowIdsTest)) ||
       (NULL == CU_add_test(pSuite2, "Test Self Join No Intermediate", createInterSelfJoinTest)))
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
