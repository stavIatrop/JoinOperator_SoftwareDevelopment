#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

#include <CUnit/Automated.h>
#include <CUnit/CUnit.h>
#include "CUnit/Basic.h"

#include "queryStructs.h"
#include "interListInterface.h"
#include "basicStructs.h"
#include "resultListInterface.h"
#include "checksumInterface.h"
#include "jointPerformer.h"
#include "I_O_structs.h"
#include "queryManip.h"
#include "jobScheduler.h"
#include "getStats.h"
#include "hashTreeManip.h"
#include "joinEnumeration.h"

#define TUPLE_NUMB 1000000
#define ROWS 64

headInter * head = NULL;
headResult * headRes = NULL;
headInter * headInt = NULL;
myint_t *arr1D, *col;
relation *rel;
colRel *cr;

Input * input = NULL;
relationsheepArray relArray;
query * newQuery = NULL;

pthread_mutex_t testMutex;
uint32_t testSchedulerValue = 0;
uint64_t calculationIntensive = 0;

myint_t * col = NULL;
stats * statsStruct = NULL;


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
    for(int whichRow = 0; whichRow < rows; whichRow++) {
        for(int whichCol = 0; whichCol < cols; whichCol++) {
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

void testUpdateInter() {
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
            temp.rowR = (myint_t) i;
            temp.rowS = (myint_t) i;

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
    myint_t ** arr = createResultArray(headRes, &size, 0);

    CU_ASSERT(size == TUPLE_NUMB);
    CU_ASSERT(arr[0][0] == 0);
    CU_ASSERT(arr[1][0] == 0);
    CU_ASSERT(arr[0][10000] == 10000);
    CU_ASSERT(arr[1][10000] == 10000);
    CU_ASSERT(arr[0][TUPLE_NUMB - 1] == TUPLE_NUMB - 1);
    CU_ASSERT(arr[1][TUPLE_NUMB - 1] == TUPLE_NUMB - 1);

    for(myint_t i = 0; i < 2; i++) {
        free(arr[i]);
    }
    free(arr);
}

void updateRowIdsTest() {
    headResult * headResLocal = initialiseResultHead();
    int rows = 100000;
    int cols = 3;
    myint_t ** rowIds = (myint_t **) malloc(cols * sizeof(myint_t *));
    for(int i = 0; i < cols; i++) {
        rowIds[i] = (myint_t *) malloc(rows * sizeof(myint_t));
    }
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            rowIds[j][i] = i;
        }
    }

    myint_t * joinedRels = (myint_t *) malloc(cols * sizeof(myint_t));
    for(int i = 0; i < cols; i++) {
        joinedRels[i] = i;
    }
    nodeInter * intNode = initialiseNode(cols, rows, joinedRels, rowIds);

    for(int i = 0; i < rows / 2; i++) {
        rowTuple temp;
        temp.rowR = (myint_t) i * 2;
        temp.rowS = (myint_t) i * 2;

        pushResult(headResLocal, &temp);
    }

    myint_t ** arr = updateRowIds(intNode, headResLocal, rows / 2, 0);

    CU_ASSERT(arr[0][0] == 0);
    CU_ASSERT(arr[1][0] == 0);
    CU_ASSERT(arr[2][0] == 0);
    CU_ASSERT(arr[3][0] == 0);
    CU_ASSERT(arr[0][rows / 4] == rows / 2);
    CU_ASSERT(arr[3][rows / 4] == rows / 2);
    CU_ASSERT(arr[1][rows / 2 - 1] == rows - 2);
    CU_ASSERT(arr[2][rows / 2 - 1] == rows - 2);
    CU_ASSERT(arr[3][rows / 2 - 1] == rows - 2);

    for(int i = 0; i < cols + 1; i++) {
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
    myint_t ** rowIds = (myint_t **) malloc(cols * sizeof(myint_t *));
    for(int i = 0; i < cols; i++) {
        rowIds[i] = (myint_t *) malloc(rows * sizeof(myint_t));
    }
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            rowIds[j][i] = i;
        }
    }

    myint_t * joinedRels = (myint_t *) malloc(cols * sizeof(myint_t));
    for(int i = 0; i < cols; i++) {
        joinedRels[i] = i;
    }

    nodeInter * node1 = initialiseNode(cols, rows, joinedRels, rowIds);

    rowIds = (myint_t **) malloc(cols * sizeof(myint_t *));
    for(int i = 0; i < cols; i++) {
        rowIds[i] = (myint_t *) malloc(rows * sizeof(myint_t));
    }
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            rowIds[j][i] = i;
        }
    }

    joinedRels = (myint_t *) malloc(cols * sizeof(myint_t));
    for(int i = 0; i < cols; i++) {
        joinedRels[i] = i + cols;
    }
    
    nodeInter * node2 = initialiseNode(cols, rows, joinedRels, rowIds);

    for(int i = 0; i < rows / 2; i++) {
        rowTuple temp;
        temp.rowR = (myint_t) i * 2;
        temp.rowS = (myint_t) i * 2;

        pushResult(headResLocal, &temp);
    }

    myint_t ** arr = joinRowIds(node1, node2, headResLocal, rows / 2, 0);

    CU_ASSERT(arr[0][0] == 0);
    CU_ASSERT(arr[2][0] == 0);
    CU_ASSERT(arr[3][0] == 0);
    CU_ASSERT(arr[0][10] == 10 * 2);
    CU_ASSERT(arr[3][10] == 10 * 2);
    CU_ASSERT(arr[0][rows / 4] == rows /2);
    CU_ASSERT(arr[2][rows / 4] == rows / 2);
    CU_ASSERT(arr[1][rows / 2 - 1] == rows - 2);
    CU_ASSERT(arr[2][rows / 2 - 1] == rows - 2);
    CU_ASSERT(arr[3][rows / 2 - 1] == rows - 2);

    for(int i = 0; i < 2 * cols; i++) {
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
    CU_ASSERT(headInt->start->data->rowIds[0][rows / 2] == rows / 2);
    CU_ASSERT(headInt->start->data->rowIds[0][rows - 1] == rows - 1);

    freeInterList(headInt);
    free(arr);
    headInt = initialiseHead();

}

void updateSelfJoinTest() {
    int rows = 1000;
    int cols = 2;
    myint_t ** rowIds = (myint_t **) malloc(cols * sizeof(myint_t *));
    for(int i = 0; i < cols; i++) {
        rowIds[i] = (myint_t *) malloc(rows * sizeof(myint_t));
    }
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            rowIds[j][i] = i;
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
    CU_ASSERT(node->data->rowIds[1][0] == 0);
    CU_ASSERT(node->data->rowIds[0][10] == 20);
    CU_ASSERT(node->data->rowIds[1][10] == 20);
    CU_ASSERT(node->data->rowIds[0][100] == 200);
    CU_ASSERT(node->data->rowIds[0][100] == 200);
    CU_ASSERT(node->data->rowIds[0][rows / 2 - 1] == rows - 2);
    CU_ASSERT(node->data->rowIds[1][rows / 2 - 1] == rows - 2);

    freeNode(node);
    free(arr);
}

void joinTwoIntermediates() {

    CU_ASSERT(headInt->numOfIntermediates == 0);

    headResult * headResLocal = initialiseResultHead();
    int rows = 100000;
    int cols = 2;
    myint_t ** rowIds = (myint_t **) malloc(cols * sizeof(myint_t *));
    for(int i = 0; i < cols; i++) {
        rowIds[i] = (myint_t *) malloc(rows * sizeof(myint_t));
    }
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            rowIds[j][i] = i;
        }
    }

    myint_t * joinedRels = (myint_t *) malloc(cols * sizeof(myint_t));
    for(int i = 0; i < cols; i++) {
        joinedRels[i] = i;
    }

    pushInter(headInt, cols, rows, joinedRels, rowIds);

    rowIds = (myint_t **) malloc(cols * sizeof(myint_t *));
    for(int i = 0; i < cols; i++) {
        rowIds[i] = (myint_t *) malloc(rows * sizeof(myint_t));
    }
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            rowIds[j][i] = i;
        }
    }

    joinedRels = (myint_t *) malloc(cols * sizeof(myint_t));
    for(int i = 0; i < cols; i++) {
        joinedRels[i] = i + cols;
    }
    
    pushInter(headInt, cols, rows, joinedRels, rowIds);

    for(int i = 0; i < rows / 2; i++) {
        rowTuple temp;
        temp.rowR = (myint_t) i * 2;
        temp.rowS = (myint_t) i * 2;

        pushResult(headResLocal, &temp);
    }

    CU_ASSERT(headInt->numOfIntermediates == 2);

    updateInterAndDelete(headInt, headInt->start, headInt->start->next, headResLocal, 0);

    CU_ASSERT(headInt->numOfIntermediates == 1);
    CU_ASSERT(headInt->start->data->numOfCols == 4);
    CU_ASSERT(headInt->start->data->numbOfRows == rows / 2);
    CU_ASSERT(headInt->start->data->joinedRels[0] == 0);
    CU_ASSERT(headInt->start->data->joinedRels[3] == 3);

    CU_ASSERT(headInt->start->data->rowIds[0][0] == 0);
    CU_ASSERT(headInt->start->data->rowIds[2][0] == 0);
    CU_ASSERT(headInt->start->data->rowIds[3][0] == 0);
    CU_ASSERT(headInt->start->data->rowIds[0][10] == 10 * 2);
    CU_ASSERT(headInt->start->data->rowIds[3][10] == 10 * 2);
    CU_ASSERT(headInt->start->data->rowIds[0][rows / 4] == rows /2);
    CU_ASSERT(headInt->start->data->rowIds[2][rows / 4] == rows / 2);
    CU_ASSERT(headInt->start->data->rowIds[1][rows / 2 - 1] == rows - 2);
    CU_ASSERT(headInt->start->data->rowIds[2][rows / 2 - 1] == rows - 2);
    CU_ASSERT(headInt->start->data->rowIds[3][rows / 2 - 1] == rows - 2);

    CU_ASSERT_PTR_NULL(headInt->start->next);

    freeInterList(headInt);
    headInt = initialiseHead();
    freeResultList(headResLocal);

}

void joinInterWithRelTest() {
    headResult * headResLocal = initialiseResultHead();
    int rows = 1000;
    int cols = 3;
    myint_t ** rowIds = (myint_t **) malloc(cols * sizeof(myint_t *));
    for(int i = 0; i < cols; i++) {
        rowIds[i] = (myint_t *) malloc(rows * sizeof(myint_t));
    }
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            rowIds[j][i] = i;
        }
    }

    myint_t * joinedRels = (myint_t *) malloc(cols * sizeof(myint_t));
    for(int i = 0; i < cols; i++) {
        joinedRels[i] = i;
    }

    nodeInter * node = initialiseNode(cols, rows, joinedRels, rowIds);

    for(int i = 0; i < rows / 2; i++) {
        rowTuple temp;
        temp.rowR = (myint_t) i * 2;
        temp.rowS = (myint_t) i * 2;

        pushResult(headResLocal, &temp);
    }

    updateInterFromRes(node, headResLocal, 5, 0);

    CU_ASSERT(node->data->numOfCols == cols + 1);
    CU_ASSERT(node->data->numbOfRows == rows / 2);
    CU_ASSERT(node->data->joinedRels[2] == 2);
    CU_ASSERT(node->data->joinedRels[3] == 5);

    CU_ASSERT(node->data->rowIds[0][0] == 0);
    CU_ASSERT(node->data->rowIds[3][0] == 0);
    CU_ASSERT(node->data->rowIds[1][10] == 20);
    CU_ASSERT(node->data->rowIds[3][100] == 200);
    CU_ASSERT(node->data->rowIds[2][rows / 4] == rows / 2);
    CU_ASSERT(node->data->rowIds[3][rows / 4] == rows / 2);
    CU_ASSERT(node->data->rowIds[0][rows / 2 - 1] == rows - 2);
    CU_ASSERT(node->data->rowIds[3][rows / 2 - 1] == rows - 2);

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
    myint_t ** rowIds = (myint_t **) malloc(cols * sizeof(myint_t *));
    for(int i = 0; i < cols; i++) {
        rowIds[i] = (myint_t *) malloc(rows * sizeof(myint_t));
    }
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            rowIds[j][i] = i;
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
    headInt = initialiseHead();
    int rows = 1000;
    int cols = 2;
    myint_t ** rowIds = (myint_t **) malloc(cols * sizeof(myint_t *));
    for(int i = 0; i < cols; i++) {
        rowIds[i] = (myint_t *) malloc(rows * sizeof(myint_t));
    }
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            rowIds[j][i] = i;
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
	freeInterList(headInt);
	free(cr);
        free(col);
	return 0;
}

/*void testRelationsheepForging()
{
	myint_t rows2 = 2000, rows = 1000;

	rel = forgeRelationsheep(headInt, cr);

	CU_ASSERT(rel->size==rows2);
	for (int i=0; i<rows2; i++)
	{
		CU_ASSERT(rel->tuples[i].key==rel->tuples[i].payload);
	}

	cr -> rel = 1;
	free(rel->tuples);
	free(rel);
	rel = forgeRelationsheep(headInt, cr);

        CU_ASSERT(rel->size==rows);
        for (int i=0; i<rows; i++)
        {
                CU_ASSERT(rel->tuples[i].key==rel->tuples[i].payload);
        }

	nodeInter *node = findNode(headInt,1);
	for (int i=0; i<rows; i++) node->data->rowIds[1][i] = node->data->rowIds[1][i] * 2;
	free(rel->tuples);
        free(rel);
        rel = forgeRelationsheep(headInt, cr);

        CU_ASSERT(rel->size==rows);
        for (int i=0; i<rows; i++)
        {
                CU_ASSERT(rel->tuples[i].key==rel->tuples[i].payload*2);
        }

	for (int i=0; i<rows; i++)
	{
		if (i%2==1) node->data->rowIds[1][i] = i - 1;
		else node->data->rowIds[1][i] = i;
	}
        free(rel->tuples);
        free(rel);
        rel = forgeRelationsheep(headInt, cr);

        CU_ASSERT(rel->size==rows);
        for (int i=0; i<rows; i++)
        {
		if (i%2==1) {CU_ASSERT(rel->tuples[i].key==rel->tuples[i-1].key)}
		else {CU_ASSERT(rel->tuples[i].key==rel->tuples[i].payload)}
        }

	free(rel->tuples);
	free(rel);
}*/

void testFilter()
{
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

int initialiseInputStructs() {

        input = InitializeInput();
        
        return 0;

}

int freeInputStructs() {

        FreeRelArray(relArray);
        FreeInput(input);
        return 0;
}

void testAddInputNode() {

        char * filename = (char *)malloc( (strlen("./small/r0") + 1) * sizeof(char));
        memset(filename, '\0', strlen("./small/r0") + 1);
        strcpy(filename, "./small/r0");
        strcat(filename, "\0");
        AddInputNode(input, filename);

        CU_ASSERT(!(strcmp(input->tail->filename, "./small/r0")));

        memset(filename, '\0', strlen("./small/r1") + 1);
        strcpy(filename, "./small/r1");
        strcat(filename, "\0");
        AddInputNode(input, filename);

        CU_ASSERT(!(strcmp(input->tail->filename, "./small/r1")));
        CU_ASSERT(input->numNodes == 2);
        CU_ASSERT(!(strcmp(input->head->filename, "./small/r0")));
        free(filename); 
        return;       

}

void testConstructInput() {

        char * inputStr = (char *) malloc( ( strlen("./small/r2\n./small/r3\n./small/r4\n./small/r5\n./small/r6\n") + 1) * sizeof(char));
        memset(inputStr, '\0', strlen("./small/r2\n./small/r3\n./small/r4\n./small/r5\n./small/r6\n") + 1);
        strcpy(inputStr, "./small/r2\n./small/r3\n./small/r4\n./small/r5\n./small/r6\n");
        strcat(inputStr, "\0");

        ConstructInput(input, inputStr);

        CU_ASSERT(!(strcmp(input->tail->filename, "./small/r6")));

        InputNode * temp = input->head;
        temp = temp->next->next;

        CU_ASSERT(!(strcmp(temp->filename, "./small/r2")));

        CU_ASSERT(input->numNodes == 7);

        free(inputStr);
        return;

}

void testFillRelArray() {

        relArray = InitializeRelSheepArray(input->numNodes);
        FillRelArray(&relArray, input);

        CU_ASSERT(relArray.rels[6].rows == 26388);
        CU_ASSERT(relArray.rels[6].cols == 2);
        CU_ASSERT(relArray.rels[0].rows == 1561);
        CU_ASSERT(relArray.rels[0].cols == 3);
        CU_ASSERT(relArray.rels[3].rows == 23038);
        CU_ASSERT(relArray.rels[3].cols == 4);

        CU_ASSERT(relArray.rels[1].pointToCols[2][301] == 6666);
        CU_ASSERT(relArray.rels[4].pointToCols[0][228] == 666);
        CU_ASSERT(relArray.rels[5].pointToCols[1][314] == 8999);
        return;

}


void testConstructQuery() {

        char * queryStr = (char *) malloc( (strlen("3 0 1|0.2=1.0&0.1=2.0&0.2>3499|1.2 0.1") + 1) * sizeof(char));
        memset(queryStr, '\0', strlen("3 0 1|0.2=1.0&0.1=2.0&0.2>3499|1.2 0.1") + 1 );
        strcpy(queryStr, "3 0 1|0.2=1.0&0.1=2.0&0.2>3499|1.2 0.1");
        strcat(queryStr, "\0");

        newQuery = ConstructQuery(queryStr, 3, 2, 2, 1, relArray);
        CU_ASSERT(newQuery->filters[0].participant.rows == 23038);
        CU_ASSERT(newQuery->joins[1].participant1.rows == 23038);
        CU_ASSERT(newQuery->joins[1].participant2.rows == 3754);

        FreeQuery(newQuery, relArray);
        free(queryStr);

        queryStr = (char *) malloc( (strlen("5 0|0.2=1.0&0.3=9881|1.1 0.2 1.0") + 1) * sizeof(char));
        memset(queryStr, '\0', strlen("5 0|0.2=1.0&0.3=9881|1.1 0.2 1.0") + 1 );
        strcpy(queryStr, "5 0|0.2=1.0&0.3=9881|1.1 0.2 1.0");
        strcat(queryStr, "\0");

        newQuery = ConstructQuery(queryStr, 2, 1, 3, 1, relArray);
        CU_ASSERT(newQuery->filters[0].participant.rows == 4910);
        CU_ASSERT(newQuery->joins[0].participant2.rows == 1561);
        CU_ASSERT(newQuery->filters[0].participant.col[4781] == 9064);
        CU_ASSERT(newQuery->sums[0].col[535] == 10005);

        FreeQuery(newQuery, relArray);
        free(queryStr);
        
        return;

}

//Test Job Scheduler
int initialiseSchedulerTest() {
    pthread_mutex_init(&testMutex, 0);
    testSchedulerValue = 0;
    initialiseScheduler();
    return 0;
}

int freeSchedulerTest() {
    shutdownAndFreeScheduler();
    pthread_mutex_destroy(&testMutex);
    return 0;
}

typedef struct ExampleArgs {
    int a;
    int b;
} exampleArgs;

void addFunction(void * args) {
    pthread_mutex_lock(&testMutex);

    //Typecasting arguments
    exampleArgs * castedArgs = (exampleArgs *) args;

    //Proccess intensive calculations
    calculationIntensive += (uint64_t) pow(testSchedulerValue, 3.0) * 3 + sqrt(((double) testSchedulerValue));
    testSchedulerValue += 1 + castedArgs->a + castedArgs->b;

    pthread_mutex_unlock(&testMutex);
}

//Does not test the speedup of threads cause of testMutex
void schedulerAdditionTest() {
    
    exampleArgs * args = (exampleArgs *) malloc(sizeof(exampleArgs));
    args->a = 1;
    args->b = 1;
    for(int i = 0; i < 100000; i++) {
        struct Job * job = (struct Job *) malloc(sizeof(struct Job));
        job->function = &addFunction;
        job->argument = (void *) args;
        writeOnQueue(job);
    }

    //Not allowing main process send a shutdown signal to the other threads for 2 seconds
    sleep(2);

    free(args);

    CU_ASSERT(testSchedulerValue == 3 * 100000);
    CU_ASSERT(calculationIntensive != 0);
}


//test FillStats

int initialiseStatistics() {

    col = (myint_t *)malloc( ROWS * sizeof(myint_t));

    statsStruct = (stats *) malloc(sizeof(stats));

    return 0;
}

int freeStatistics() {

    free(col);
    
    free(statsStruct);

    return 0;
}

void fillStatsTest() {

    for(int i = 0; i < ROWS; i++) {
        col[i] = 1;
    }
    FillStatsArray(col, statsStruct, ROWS);

    CU_ASSERT(statsStruct->distinctVals == 1);

    free(statsStruct->distinctArray);

    for(int i = 0; i < ROWS; i++) {
        col[i] = i;
    }

    FillStatsArray(col, statsStruct, ROWS);
    
    CU_ASSERT(statsStruct->distinctVals == ROWS);

    free(statsStruct->distinctArray);


    for(int i = 0; i < ROWS; i++) {

        col[i] = i % 2;
        
    }

    FillStatsArray(col, statsStruct, ROWS);

    CU_ASSERT(statsStruct->distinctVals == 2);

    free(statsStruct->distinctArray);

     for(int i = 0; i < ROWS; i++) {

        col[i] = i % 4;
        
    }

    FillStatsArray(col, statsStruct, ROWS);

    CU_ASSERT(statsStruct->distinctVals == 4);

    free(statsStruct->distinctArray);


    return;

}

void testCombToIndexFunc() {

    myint_t index = combToIndex("102");

    CU_ASSERT(index == 7);

    index = combToIndex("032");

    CU_ASSERT(index == 13);

    index = combToIndex("03");

    CU_ASSERT(index == 9);

    return;
}

void testExistsInComb() {

    myint_t retVal = existsInComb("1234", 3);
    CU_ASSERT(retVal == 1);

    retVal = existsInComb("1", 3);
    CU_ASSERT(retVal == 0);

    retVal = existsInComb("2", 2);
    CU_ASSERT(retVal == 1);

    retVal = existsInComb("01234", 4);
    CU_ASSERT(retVal == 1);
    return; 
}

int main(void) {

	CU_pSuite pSuite1 = NULL;
	CU_pSuite pSuite2 = NULL;
	CU_pSuite pSuite3 = NULL;
	CU_pSuite pSuite4 = NULL;
	//CU_pSuite pSuite5 = NULL;
	CU_pSuite pSuite6 = NULL;
	CU_pSuite pSuite7 = NULL;
	CU_pSuite pSuite8 = NULL;
    CU_pSuite pSuite9 = NULL;

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
       (NULL == CU_add_test(pSuite1, "Test Updating", testUpdateInter)) ||
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

  	pSuite4 = CU_add_suite("Test Next Row Id", initArray, freeArray);
        if (NULL == pSuite4) {
                CU_cleanup_registry();
                return CU_get_error();
        }
        if (NULL == CU_add_test(pSuite4, "Test it", testNextRowId))
        {
                CU_cleanup_registry();
                return CU_get_error();
        }

	/*pSuite5 = CU_add_suite("Test Forging Operations", smarterInit, smarterFree);
        if (NULL == pSuite5) {
                CU_cleanup_registry();
                return CU_get_error();
        }
        if (NULL == CU_add_test(pSuite5, "Test 1", testRelationsheepForging))
        {
                CU_cleanup_registry();
                return CU_get_error();
        }*/

	pSuite6 = CU_add_suite("Test Filtering", smarterInit, smarterFree);
        if (NULL == pSuite6) {
                CU_cleanup_registry();
                return CU_get_error();
        }
        if (NULL == CU_add_test(pSuite6, "Test 1", testFilter))
        {
                CU_cleanup_registry();
                return CU_get_error();
        }

   pSuite7 = CU_add_suite("I_O Suite", initialiseInputStructs, freeInputStructs);
   if (NULL == pSuite7) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite7, "Test Adding Input Node at Input List", testAddInputNode)) ||
       (NULL == CU_add_test(pSuite7, "Test Constructing Input List", testConstructInput)) ||
       (NULL == CU_add_test(pSuite7, "Test Filling RelationsheepArray", testFillRelArray)) ||
       (NULL == CU_add_test(pSuite7, "Test Constructing Query Struct", testConstructQuery)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

    pSuite8 = CU_add_suite("Test Scheduler", initialiseSchedulerTest, freeSchedulerTest);
    if (NULL == pSuite8) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite8, "Test addition job", schedulerAdditionTest)))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    pSuite9 = CU_add_suite("Test Statistics", initialiseStatistics, freeStatistics);
    if (NULL == pSuite9) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite9, "Test fillStats", fillStatsTest)) ||
       (NULL == CU_add_test(pSuite9, "Test CombToIndex", testCombToIndexFunc)) ||
       (NULL == CU_add_test(pSuite9, "Test existsInComb", testExistsInComb)) )
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
