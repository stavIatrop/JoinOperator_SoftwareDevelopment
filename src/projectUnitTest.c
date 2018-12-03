#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <CUnit/Automated.h>
#include <CUnit/CUnit.h>
#include "CUnit/Basic.h"

#include "inputInterface.h"
#include "basicStructs.h"
#include "resultListInterface.h"
#include "viceFunctions.h"
#include "indexManip.h"
#include "sideFunctions.h"
#include "hashing.h"
#include "chainFollowerInterface.h"
#include "resultListInterface.h"

#define OUTPUT_SIZE 200000
#define NUM_OF_TUPLES_A 10
#define HASH1_A 3
#define HASH2_A 4
#define PSUMSIZE 3
#define NUM_OF_TUPLES 15
#define HASH1 3
#define HASH2 5
#define TUPLE_NUMB 10000000

FILE * inputFile = NULL;
FILE * outputFile = NULL;
table * t = NULL;
headResult * list_a;
relation *r1,*r2,*r3;

myint_t * chain_a = NULL;
myint_t * buckets_a = NULL;

relationIndex * oneIndex_a;

reorderedR * ror_a = NULL;
indexArray * indexA_a = NULL;
relationIndex * newIndex = NULL;
tuple * startOfBucket = NULL;

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

//Test readAsciiTable
int initAsciiTable() {
        t = (table *) malloc(sizeof(table));
        t->columns = 3;
        t->rows = 10;
        inputFile  = fopen("smallTestTables/file3_10ascii", "r");

        //Burn the first line
        char * firstLine = NULL;
        size_t len = 0;
        if(getline(&firstLine, &len, inputFile) == -1) {
                return -1;
        }
        free(firstLine);
        t->content = (myint_t **) malloc(t->columns * sizeof(myint_t *));
        for(int i = 0; i < t->columns; i++) {
                t->content[i] = (myint_t *) calloc(t->rows, sizeof(myint_t));
        }
        return 0;
}

int freeAsciiTable() {
        fclose(inputFile);
        for(int i = 0; i < t->columns; i++) {
                free(t->content[i]);
        }
        free(t->content);
        free(t);

        return 0;
}

void testAsciiTable() {
        int res = readAsciiTable(t, inputFile);
        CU_ASSERT(res == 0);

        CU_ASSERT(t->content[0][0] == 426099812);
        CU_ASSERT(t->content[t->columns - 1][t->rows - 1] == 1065031800);
        CU_ASSERT(t->content[0][2] == 1330485503);
        CU_ASSERT(t->content[1][3] == 411261800);
        CU_ASSERT(t->content[2][8] == 1374752365);

}

//Test readBinTable
int initBinTable() {
        t = (table *) malloc(sizeof(table));
        t->columns = 3;
        t->rows = 10;
        inputFile  = fopen("smallTestTables/file3_10", "r");

        //Burn the first line
        char * firstLine = NULL;
        size_t len = 0;
        if(getline(&firstLine, &len, inputFile) == -1) {
                return -1;
        }
        free(firstLine);

        t->content = (myint_t **) malloc(t->columns * sizeof(myint_t *));
        for(int i = 0; i < t->columns; i++) {
                t->content[i] = (myint_t *) calloc(t->rows, sizeof(myint_t));
        }

        return 0;
}

int freeBinTable() {
        fclose(inputFile);
        for(int i = 0; i < t->columns; i++) {
                free(t->content[i]);
        }
        free(t->content);
        free(t);

        return 0;
}

void testBinTable() {
        int res = readBinTable(t, inputFile);
        CU_ASSERT(res == 0);

        CU_ASSERT(t->content[0][0] == 1904377912);
        CU_ASSERT(t->content[t->columns - 1][t->rows - 1] == 1613197574);
        CU_ASSERT(t->content[0][2] == 1178377593);
        CU_ASSERT(t->content[1][3] == 2117763694);
        CU_ASSERT(t->content[2][8] == 1017926526);

}


//Test readTable
void testReadAscii() {
        t = readTable("smallTestTables/file3_10ascii", ASCII_FILE);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT(t->columns == 3);
        CU_ASSERT(t->rows == 10);
        CU_ASSERT(t->content[0][0] == 426099812);
        CU_ASSERT(t->content[t->columns - 1][t->rows - 1] == 1065031800);
        CU_ASSERT(t->content[0][2] == 1330485503);
        CU_ASSERT(t->content[1][3] == 411261800);
        CU_ASSERT(t->content[2][8] == 1374752365);

        freeTable(t);
}

void testReadBin() {
        t = readTable("smallTestTables/file3_10", BINARY_FILE);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT(t->columns == 3);
        CU_ASSERT(t->rows == 10);
        CU_ASSERT(t->content[0][0] == 1904377912);
        CU_ASSERT(t->content[t->columns - 1][t->rows - 1] == 1613197574);
        CU_ASSERT(t->content[0][2] == 1178377593);
        CU_ASSERT(t->content[1][3] == 2117763694);
        CU_ASSERT(t->content[2][8] == 1017926526);

        freeTable(t);
}

//Test Extract Relation
void testExtractRelation() {
        t = readTable("smallTestTables/file3_10ascii", ASCII_FILE);
        relation * r = extractRelation(t, 1);

        CU_ASSERT(r->size == 10);

        CU_ASSERT(r->tuples[0].key == 1482191081);
        CU_ASSERT(r->tuples[0].payload == 0);

        CU_ASSERT(r->tuples[4].key == 1727628515);
        CU_ASSERT(r->tuples[4].payload == 4);

        CU_ASSERT(r->tuples[9].key == 1101437841);
        CU_ASSERT(r->tuples[9].payload == 9);

        free(r->tuples);
        free(r);
        freeTable(t);
}

//Test Write Output
int initOutput() {
        list_a = initialiseResultHead();
        for(myint_t i = 0; i < OUTPUT_SIZE; i++) {
                rowTuple temp;
                temp.rowR = i;
                temp.rowS = i;
                pushResult(list_a, &temp);
        }
        writeList(list_a, "smallTestTables/out_bin");

        if((outputFile = fopen("smallTestTables/out_bin", "r")) == NULL) {
                return -1;
        }
        return 0;
}

int freeOutput() {
        freeResultList(list_a);
        fclose(outputFile);

        return 0;
}

void checkSize() {
        char * firstLine = NULL;
        size_t len = 0;
        getline(&firstLine, &len, outputFile);

        int size = atoi(strtok(firstLine, "\n"));
        CU_ASSERT(size == OUTPUT_SIZE);
        free(firstLine);
}

void checkTuples() {
        tuple * tuples = calloc(OUTPUT_SIZE, sizeof(tuple));
        int totalReads = 0, reads = 0;

        while((reads = fread(tuples + totalReads * sizeof(tuple), sizeof(tuple), OUTPUT_SIZE, outputFile)) + totalReads != OUTPUT_SIZE) {
                if(reads <= 0) {
                        CU_ASSERT(1 == 0);
                        break;
                }
                totalReads += reads;
        }

        CU_ASSERT(reads == OUTPUT_SIZE);
        CU_ASSERT(tuples[0].key == 0);
        CU_ASSERT(tuples[0].payload == 0);
        CU_ASSERT(tuples[50000].key == 50000);
        CU_ASSERT(tuples[50000].payload == 50000);
        CU_ASSERT(tuples[135678].key == 135678);
        CU_ASSERT(tuples[135678].payload == 135678);
        CU_ASSERT(tuples[OUTPUT_SIZE - 1].key == OUTPUT_SIZE - 1);
        CU_ASSERT(tuples[OUTPUT_SIZE - 1].payload == OUTPUT_SIZE - 1);

        free(tuples);
}

void Powerful()
{
	CU_ASSERT(FindNextPower(65)==128);
	CU_ASSERT(FindNextPower(0)==1);
	CU_ASSERT(FindNextPower(64)==64);
	CU_ASSERT(FindNextPower((2<<29)+1)==2<<30);
	CU_ASSERT(FindNextPower(1)==1);
	CU_ASSERT(FindNextPower(3)==4);
}

int initIdentical()
{
	r3 = malloc(sizeof(relation));
	r3->tuples = malloc(10000000 * sizeof(tuple));
	r3->size = 10000000;
	for (myint_t i=0; i<10000000; i++)
	{
		if (i%10==0) r3->tuples[i].key=0;
		else r3->tuples[i].key=i;
	}
	return 0;
}

int freeIdentical()
{
	free(r3->tuples);
	free(r3);
	return 0;
}

void Cloning()
{
	CU_ASSERT(IdenticalityTest(r3) >= 0.09 && IdenticalityTest(r3) <= 0.11);
}

//<-- Indexing tests -->
int InitializeRor() {

	ror_a = (reorderedR *) malloc(sizeof(reorderedR));
	
	ror_a->pSumArr.psumSize = PSUMSIZE;
	ror_a->pSumArr.psum = (pSumTuple *) malloc(PSUMSIZE * sizeof(pSumTuple));
	int i;
	for(i = 0; i < PSUMSIZE; i++) {
		ror_a->pSumArr.psum[i].h1Res = i;
		ror_a->pSumArr.psum[i].offset = i * 5;
	}

	ror_a->rel = (relation *) malloc(sizeof(relation));
	ror_a->rel->size = 15;
	ror_a->rel->tuples = (tuple *) malloc(15 * sizeof(tuple));

	int j = 3;
	for( i = 0; i < 5; i++) {
		ror_a->rel->tuples[i].payload = j;
		ror_a->rel->tuples[i].key = j;
		j += 3; 
	}

	j = 1;
	for( i = 5; i < 10; i++) {
		ror_a->rel->tuples[i].payload = j;
		ror_a->rel->tuples[i].key = j;
		j += 3; 
	}	

	j = 2;
	for( i = 10; i < 15; i++) {
		ror_a->rel->tuples[i].payload = j;
		ror_a->rel->tuples[i].key = j;
		j += 3; 
	}	

	startOfBucket = (tuple *) malloc(NUM_OF_TUPLES_A * sizeof(tuple));
	for(i = 0; i < 10; i++) {

		startOfBucket[i].payload = i;
		startOfBucket[i].key = (i + 1) * 25;
	}


	return 0;

}

void RecFreeIndex(relationIndex * rI) {

	relationIndex * temp = rI;

	if(temp->next != NULL) 
		RecFreeIndex(temp->next);

	free(temp->chain);
	free(temp->buckets);
	free(temp->rel);
	free(temp);
	return;
}

int freeRor() {

	free(ror_a->rel->tuples);
	free(ror_a->rel);
	free(ror_a->pSumArr.psum);
	free(ror_a);
	free(startOfBucket);
	RecFreeIndex(newIndex);
	return 0;
}



void testStartOfBucket() {

	relation *rel = getStartOfBucket(ror_a, 1);

	CU_ASSERT(rel->tuples[0].key == 1);
	free(rel);
	rel = getStartOfBucket(ror_a, 2);
	CU_ASSERT(rel->tuples[3].key == 11);
	free(rel);

	rel = getStartOfBucket(ror_a, 0);
	CU_ASSERT(rel->tuples[1].key == 6);
	free(rel);

	return;
}

void testBuildSubIndex() {

	myint_t sizeAll = 10, eachSize = 3, key = 10, sizeIndexedSofar = 3;
	sizeAll -= eachSize;
	buildSubIndex(&newIndex, HASH1_A, HASH2_A, sizeAll, eachSize, sizeIndexedSofar, startOfBucket, key);
	
	relationIndex * temp = newIndex;

	CU_ASSERT(temp->buckets[12] == 1);
	CU_ASSERT(temp->buckets[15] == 2);
	CU_ASSERT(temp->buckets[2] == 3);

	for(int i = 0; i < eachSize; i++ )
		CU_ASSERT(temp->chain[i] == 0);

	temp = temp->next;

	CU_ASSERT(temp->buckets[5] == 1);
	CU_ASSERT(temp->buckets[9] == 2);
	CU_ASSERT(temp->buckets[12] == 3);
	for(int i = 0; i < eachSize; i++ )
		CU_ASSERT(temp->chain[i] == 0);

	temp = temp->next;

	CU_ASSERT(temp->buckets[15] == 1);
	
	for(int i = 0; i < 1; i++ )
		CU_ASSERT(temp->chain[i] == 0);

	return;
}


int InitializeIndexTest() {

	relation * rel = (relation *) malloc(sizeof(relation));
	rel->size = NUM_OF_TUPLES_A;
	rel->tuples = (tuple *) malloc(NUM_OF_TUPLES_A * sizeof(tuple));
	int i;
	for( i = 0; i < NUM_OF_TUPLES_A; i++){
		rel->tuples[i].payload = i;
		rel->tuples[i].key = pow(i, 2);
	}

	int bucketSize = hash2Range(HASH2_A);
	oneIndex_a = (relationIndex *) malloc(sizeof(relationIndex));
	*oneIndex_a = initializeIndex(bucketSize, rel, 6, NULL, HASH2_A);

	return 0;

}

int freeIndexTest() {

	free(oneIndex_a->chain);
	free(oneIndex_a->buckets);
	free(oneIndex_a->rel->tuples);
	free(oneIndex_a->rel);
	free(oneIndex_a);
	return 0;
}

void testBuildIndex(){


	buildIndex(oneIndex_a, HASH1_A, HASH2_A);

	int i;
	CU_ASSERT(oneIndex_a->chain[0]== 0);
	CU_ASSERT(oneIndex_a->chain[1]== 1);
	CU_ASSERT(oneIndex_a->chain[2]== 2);
	for(i = 3; i < 10; i++) {
		CU_ASSERT(oneIndex_a->chain[i] == 0);
	}
	
	
	CU_ASSERT(oneIndex_a->buckets[0] == 3);
	CU_ASSERT(oneIndex_a->buckets[1] == 4);
	CU_ASSERT(oneIndex_a->buckets[2] == 5);
	CU_ASSERT(oneIndex_a->buckets[3] == 6);
	CU_ASSERT(oneIndex_a->buckets[4] == 7);
	CU_ASSERT(oneIndex_a->buckets[5] == 0);
	CU_ASSERT(oneIndex_a->buckets[6] == 8);
	CU_ASSERT(oneIndex_a->buckets[7] == 0);
	CU_ASSERT(oneIndex_a->buckets[8] == 9);
	CU_ASSERT(oneIndex_a->buckets[9] == 0);
	CU_ASSERT(oneIndex_a->buckets[10] == 10);

	for(i = 11; i < 16; i++)
		CU_ASSERT(oneIndex_a->buckets[i] == 0);

	return;
}

int InitializeStructs() {

	chain_a = (myint_t *) malloc(8 * sizeof(myint_t));
	buckets_a = (myint_t *) malloc(5 * sizeof(myint_t));

	int i;
	for(i = 0; i < 8; i++)
		chain_a[i] = 0;
	for(i = 0; i < 5; i++)
		buckets_a[i] = 0;

	
	return 0;

}

int freeStructs() {

	free(chain_a);
	free(buckets_a);
	return 0;
}

void testUpdateChain() {

	
	updateChain(chain_a, buckets_a, 3, 2);
	updateChain(chain_a, buckets_a, 3, 3);
	updateChain(chain_a, buckets_a, 3, 6);

	CU_ASSERT(chain_a[2] == 0);
	CU_ASSERT(chain_a[3] == 3);
	CU_ASSERT(chain_a[6] == 4);
	CU_ASSERT(buckets_a[3] == 7);
	return;
}

void testHashing() {

	myint_t value = 9;
	value = hashing(value, HASH1_A, HASH2_A);
	CU_ASSERT(value == 1);
	value = 81;
	value = hashing(value, HASH1_A, HASH2_A);
	CU_ASSERT(value == 10);

	value = 64;
	value = hashing(value, HASH1_A, HASH2_A);
	CU_ASSERT(value == 8);

	value = 49;
	value = hashing(value, HASH1_A, HASH2_A);
	CU_ASSERT(value == 6);

	value = 666;
	value = hashing(value, HASH1_A, HASH2_A);
	CU_ASSERT(value == 2);
	
	value = 2147483;
	value = hashing(value, HASH1_A, HASH2_A);
	CU_ASSERT(value == 9);
	

	return;
}

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

        list = search(indexA, RoS);

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

int main(void) {

	CU_pSuite pSuite1 = NULL;
	CU_pSuite pSuite2 = NULL;
	CU_pSuite pSuite3 = NULL;
	CU_pSuite pSuite4 = NULL;
	CU_pSuite pSuite5 = NULL;
        CU_pSuite pSuite6 = NULL;
	CU_pSuite pSuite7 = NULL;
        CU_pSuite pSuite8 = NULL;
	CU_pSuite pSuite9 = NULL;
	CU_pSuite pSuite10 = NULL;
	CU_pSuite pSuite11 = NULL;
        CU_pSuite pSuite12 = NULL;
   	CU_pSuite pSuite13 = NULL;

	//Initialize the CUnit test registry
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   pSuite1 = CU_add_suite("Read Ascii Table", initAsciiTable, freeAsciiTable);
   if (NULL == pSuite1) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite1, "Test Values", testAsciiTable)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite2 = CU_add_suite("Read Binary Table", initBinTable, freeBinTable);
   if (NULL == pSuite2) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite2, "Test Values", testBinTable)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite3 = CU_add_suite("Test Read Table", NULL, NULL);
   if (NULL == pSuite3) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite3, "Test Read of Type ASCII", testReadAscii )) ||
   		(NULL == CU_add_test(pSuite3, "Test Read of Type Binary", testReadBin )))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite4 = CU_add_suite("Test Extract Relation", NULL, NULL);
   if (NULL == pSuite4) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite4, "Test Values", testExtractRelation)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite5 = CU_add_suite("Test Write Output", initOutput, freeOutput);
   if (NULL == pSuite5) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite5, "Test Size of Output", checkSize)) ||
        (NULL == CU_add_test(pSuite5, "Test Values of Output", checkTuples)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite6 = CU_add_suite("Power Test", NULL, NULL);
   if (NULL == pSuite6) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite7 = CU_add_suite("Eye Test", initIdentical, freeIdentical);
   if (NULL == pSuite7) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite6, "Test Values", Powerful)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite7, "Test Values", Cloning)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

/* add a suite to the registry */
   pSuite8 = CU_add_suite("chain_a Suite", InitializeStructs, freeStructs);
   if (NULL == pSuite8) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite8, "Test updateChain", testUpdateChain)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add a 2nd suite to the registry */
   pSuite9 = CU_add_suite("BuildIndex Suite", InitializeIndexTest, freeIndexTest);
   if (NULL == pSuite9) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the 2nd suite */
   if ((NULL == CU_add_test(pSuite9, "Test buildIndex", testBuildIndex)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite10 = CU_add_suite("Indexing Suite", InitializeRor, freeRor);
   if (NULL == pSuite10) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite10, "Test getStartOfBucket", testStartOfBucket )) ||
   		(NULL == CU_add_test(pSuite10, "Test buildSubIndex", testBuildSubIndex )))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite11 = CU_add_suite("Hashing Suite", NULL, NULL);
   if (NULL == pSuite11) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite11, "Test hashing", testHashing )) )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite12 = CU_add_suite("Result List Suite", createList, freeList);
   if (NULL == pSuite12) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite12, "Test Number of Nodes", testNumbOfNodes)) ||
       (NULL == CU_add_test(pSuite12, "Test Specific Values", testValues)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite13 = CU_add_suite("Search Suite", initialiseRelations, freeRelations);
   if (NULL == pSuite13) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite13, "Test No Matching Values", testZeroResults)) ||
       (NULL == CU_add_test(pSuite13, "Test All Matching Values Ordered", testOrderedResults)) ||
       (NULL == CU_add_test(pSuite13, "Test All Matching Values Inversed Ordered", testInversedOrderedResults)))
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
