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
#include "checksumInterface.h"
#include "jointPerformer.h"

#define TUPLE_NUMB 1000000

headInter * head = NULL;
headResult * headRes = NULL;
headInter * headInt = NULL;
myint_t *arr1D, *col;
relation *rel;
colRel *cr;

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
    int rows = 100000;
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

void updateSelfJoinTest() {
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

    nodeInter * node = initialiseNode(cols, rows, joinedRels, rowIds);

    myint_t * arr = (myint_t *) malloc((rows / 2) * sizeof(myint_t));
    for(int i = 0; i < rows / 2; i++) {
        arr[i] = i * 2;
    }

    updateInterSelfJoin(node, arr, rows / 2);

    CU_ASSERT(node->data->numbOfRows == rows / 2);
    CU_ASSERT(node->data->joinedRels[1] == 1);
    CU_ASSERT(node->data->numOfCols == 2);
    CU_ASSERT(node->data->rowIds[0][0] == 0);
    CU_ASSERT(node->data->rowIds[0][1] == 0);
    CU_ASSERT(node->data->rowIds[10][0] == 20);
    CU_ASSERT(node->data->rowIds[10][1] == 20);
    CU_ASSERT(node->data->rowIds[100][0] == 200);
    CU_ASSERT(node->data->rowIds[100][0] == 200);
    CU_ASSERT(node->data->rowIds[rows / 2 - 1][0] == rows - 2);
    CU_ASSERT(node->data->rowIds[rows / 2 - 1][1] == rows - 2);

    freeNode(node);
    free(arr);
}

void joinTwoIntermediates() {

    CU_ASSERT(headInt->numOfIntermediates == 0);

    headResult * headResLocal = initialiseResultHead();
    int rows = 100000;
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

    pushInter(headInt, cols, rows, joinedRels, rowIds);

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
    
    pushInter(headInt, cols, rows, joinedRels, rowIds);

    for(int i = 0; i < rows / 2; i++) {
        rowTuple temp;
        temp.rowR = (int32_t) i * 2;
        temp.rowS = (uint32_t) i * 2;

        pushResult(headResLocal, &temp);
    }

    CU_ASSERT(headInt->numOfIntermediates == 2);

    updateInterAndDelete(headInt, headInt->start, headInt->start->next, headResLocal);

    CU_ASSERT(headInt->numOfIntermediates == 1);

    CU_ASSERT(headInt->start->data->numOfCols == 4);
    CU_ASSERT(headInt->start->data->numbOfRows == rows / 2);
    CU_ASSERT(headInt->start->data->joinedRels[0] == 0);
    CU_ASSERT(headInt->start->data->joinedRels[3] == 3);

    CU_ASSERT(headInt->start->data->rowIds[0][0] == 0);
    CU_ASSERT(headInt->start->data->rowIds[0][2] == 0);
    CU_ASSERT(headInt->start->data->rowIds[0][3] == 0);
    CU_ASSERT(headInt->start->data->rowIds[10][0] == 10 * 2);
    CU_ASSERT(headInt->start->data->rowIds[10][3] == 10 * 2);
    CU_ASSERT(headInt->start->data->rowIds[rows / 4][0] == rows /2);
    CU_ASSERT(headInt->start->data->rowIds[rows / 4][2] == rows / 2);
    CU_ASSERT(headInt->start->data->rowIds[rows / 2 - 1][1] == rows - 2);
    CU_ASSERT(headInt->start->data->rowIds[rows / 2 - 1][2] == rows - 2);
    CU_ASSERT(headInt->start->data->rowIds[rows / 2 - 1][3] == rows - 2);

    CU_ASSERT_PTR_NULL(headInt->start->next);

    freeInterList(headInt);
    headInt = initialiseHead();
    freeResultList(headResLocal);

}

void joinInterWithRelTest() {
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

    nodeInter * node = initialiseNode(cols, rows, joinedRels, rowIds);

    for(int i = 0; i < rows / 2; i++) {
        rowTuple temp;
        temp.rowR = (int32_t) i * 2;
        temp.rowS = (uint32_t) i * 2;

        pushResult(headResLocal, &temp);
    }

    updateInterFromRes(node, headResLocal, 5);

    CU_ASSERT(node->data->numOfCols == cols + 1);
    CU_ASSERT(node->data->numbOfRows == rows / 2);
    CU_ASSERT(node->data->joinedRels[2] == 2);
    CU_ASSERT(node->data->joinedRels[3] == 5);

    CU_ASSERT(node->data->rowIds[0][0] == 0);
    CU_ASSERT(node->data->rowIds[0][3] == 0);
    CU_ASSERT(node->data->rowIds[10][1] == 20);
    CU_ASSERT(node->data->rowIds[10][3] == 20);
    CU_ASSERT(node->data->rowIds[rows / 4][2] == rows / 2);
    CU_ASSERT(node->data->rowIds[rows / 4][3] == rows / 2);
    CU_ASSERT(node->data->rowIds[rows / 2 - 1][0] == rows - 2);
    CU_ASSERT(node->data->rowIds[rows / 2 - 1][3] == rows - 2);

    freeNode(node);
    freeResultList(headResLocal);
}



//Test perform checksum
int initChecksum() {

    headInt = initialiseHead();

    return 0;
}

int freeChecksum() {
    freeInterList(headInt);
    return 0;
}


void checksumTest() {
    int rows = 100000;
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

    pushInter(headInt, cols, rows, joinedRels, rowIds);

    colRel * sums = (colRel *) malloc(cols * sizeof(colRel));
    for(int i = 0; i < cols; i++) {
        sums[i].rel = cols - i - 1;
        sums[i].col = (myint_t *) malloc(2 * rows * sizeof(myint_t));
        for(int j = 0; j < 2 * rows; j++) {
            sums[i].col[j] = i + 1;
        }
        sums[i].rows = 2 * rows;
    }


    checksum * csum = performChecksums(sums, cols, headInt);

    CU_ASSERT(csum->numbOfChecksums == 3);
    CU_ASSERT(csum->checksums[0] == rows);
    CU_ASSERT(csum->checksums[1] == 2 * rows);
    CU_ASSERT(csum->checksums[2] == 3 * rows);

    for(int i = 0; i < cols; i++) {
        free(sums[i].col);
    }
    free(sums);
    free(csum->checksums);
    free(csum);
}

int initArray()
{
        arr1D = create_1DArray(20,3);
        arr1D[5]=7;
	return 0;
}

int freeArray()
{
        free(arr1D);
	return 0;
}

void testNextRowId()
{
        CU_ASSERT(findNextRowId(arr1D,0,20)==5);
        CU_ASSERT(findNextRowId(arr1D,5,20)==6);
        CU_ASSERT(findNextRowId(arr1D,6,20)==20);
}

int smarterInit()
{
    initList();
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

    pushInter(headInt, cols, rows, joinedRels, rowIds);
    pushInter(headInt, 0, 0, NULL, NULL);

	myint_t rows2 = 2000;
        col = malloc(rows2*sizeof(myint_t));
        for (int i=0; i<rows2; i++)
        {
                col[i]=i;
        }
        cr = malloc(sizeof(colRel));
        cr -> rel = 5;
        cr -> rows = rows2;
        cr->col = col;
	return 0;
}

int smarterFree()
{
	freeList();
	free(cr);
        free(col);
	return 0;
}

void testRelationsheepForging()
{
	myint_t rows2 = 2000, rows = 1000, cols = 2;

	rel = forgeRelationsheep(headInt, cr);

	CU_ASSERT(rel->size==rows2);
	printf("Size is %d\n", rel->size);
	for (int i=0; i<rows2; i++)
	{
		CU_ASSERT(rel->tuples[i].key==rel->tuples[i].payload);
	}

	/*cr -> rel = 1;
	free(rel->tuples);
	free(rel);
	rel = forgeRelationsheep(headInt, cr);

        CU_ASSERT(rel->size==rows);
        printf("Size is %d\n", rel->size);
        for (int i=0; i<rows; i++)
        {
                CU_ASSERT(rel->tuples[i].key==rel->tuples[i].payload);
        }

	for (int i=0; i<rows; i++) rowIds[i]= rowIds[i] * 2;
	free(rel->tuples);
        free(rel);
        rel = forgeRelationsheep(headInt, cr);

        CU_ASSERT(rel->size==rows);
        printf("Size is %d\n", rel->size);
        for (int i=0; i<rows; i++)
        {
                CU_ASSERT(rel->tuples[i].key==rel->tuples[i].payload);
        }*/

	free(rel->tuples);
	free(rel);
}

void testFilter()
{
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

    nodeInter * node = initialiseNode(cols, rows, joinedRels, rowIds);

    pushInter(headInt, cols, rows, joinedRels, rowIds);
    pushInter(headInt, 0, 0, NULL, NULL);

	myint_t rows2 = 2000;
        myint_t *col = malloc(rows2*sizeof(myint_t));
        for (int i=0; i<rows2; i++)
        {
                col[i]=i;
        }
        colRel *cr = malloc(sizeof(colRel));
        cr -> rel = 5;
        cr -> rows = rows2;
        cr->col = col;

	filter filt;
	filt.participant = *cr;
	filt.op = 1;
	filt.value = 1000;
	workerF(&filt, headInt);

	CU_ASSERT(headInt->numOfIntermediates==3);
	CU_ASSERT(findNode(headInt,cr->rel)->data->numOfCols==1);
	CU_ASSERT(findNode(headInt,cr->rel)->data->numbOfRows==filt.value);
	for (int i =0; i<filt.value; i++) CU_ASSERT(findNode(headInt,cr->rel)->data->rowIds[0][i]==i);
}

int main(void) {

	CU_pSuite pSuite1 = NULL;
	CU_pSuite pSuite2 = NULL;
	CU_pSuite pSuite3 = NULL;
	CU_pSuite pSuite4 = NULL;
	CU_pSuite pSuite5 = NULL;

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
       (NULL == CU_add_test(pSuite2, "Test Self Join No Intermediate", createInterSelfJoinTest)) ||
       (NULL == CU_add_test(pSuite2, "Test Self Join With Intermediate", updateSelfJoinTest)) ||
       (NULL == CU_add_test(pSuite2, "Test Join Of Two Intermediates", joinTwoIntermediates)) ||
       (NULL == CU_add_test(pSuite2, "Test Join Of Intermediate with Relationship", joinInterWithRelTest)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add a suite to the registry */
   pSuite3 = CU_add_suite("Test Checksum", initChecksum, freeChecksum);
   if (NULL == pSuite3) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite3, "Test Checksum return values", checksumTest)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

  	pSuite4 = CU_add_suite("Test Next Row Id", smarterInit, smarterFree);
        if (NULL == pSuite4) {
                CU_cleanup_registry();
                return CU_get_error();
        }
        if (NULL == CU_add_test(pSuite4, "Test it", testNextRowId))
        {
                CU_cleanup_registry();
                return CU_get_error();
        }

	pSuite5 = CU_add_suite("Test Forging Operations", initIntermediatesResults, freeIntermedieatesResults);
        if (NULL == pSuite5) {
                CU_cleanup_registry();
                return CU_get_error();
        }
        if (NULL == CU_add_test(pSuite5, "Test 1", testRelationsheepForging))
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
