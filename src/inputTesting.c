#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <CUnit/Automated.h>
#include <CUnit/CUnit.h>
#include "CUnit/Basic.h"

#include "inputInterface.h"
#include "basicStructs.h"

FILE * inputFile = NULL;
table * t = NULL;

//Test readAsciiTable
int initAsciiTable() {
        t = (table *) malloc(sizeof(table));
        t->columns = 3;
        t->rows = 10;
        inputFile  = fopen("smallTestTables/file3_10ascii", "r");

        //Burn the first line
        char * firstLine = malloc(500);
        size_t len;
        if(getline(&firstLine, &len, inputFile) == -1) {
                return -1;
        }
        free(firstLine);
        t->content = (int32_t **) malloc(t->columns * sizeof(int32_t *));
        for(int i = 0; i < t->columns; i++) {
                t->content[i] = (int32_t *) calloc(t->rows, sizeof(int32_t));
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
        char * firstLine = malloc(500);
        size_t len;
        if(getline(&firstLine, &len, inputFile) == -1) {
                return -1;
        }
        free(firstLine);

        t->content = (int32_t **) malloc(t->columns * sizeof(int32_t *));
        for(int i = 0; i < t->columns; i++) {
                t->content[i] = (int32_t *) calloc(t->rows, sizeof(int32_t));
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

int main(void) {

	CU_pSuite pSuite1 = NULL;
	CU_pSuite pSuite2 = NULL;
	CU_pSuite pSuite3 = NULL;
	CU_pSuite pSuite4 = NULL;


	//Initialize the CUnit test registry
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

  /* add a suite to the registry */
   pSuite1 = CU_add_suite("Read Ascii Table", initAsciiTable, freeAsciiTable);
   if (NULL == pSuite1) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite1, "Test Values", testAsciiTable)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add a 2nd suite to the registry */
   pSuite2 = CU_add_suite("Read Binary Table", initBinTable, freeBinTable);
   if (NULL == pSuite2) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the 2nd suite */
   if ((NULL == CU_add_test(pSuite2, "Test Values", testBinTable)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add a 3rd suite to the registry */
   pSuite3 = CU_add_suite("Test Read Table", NULL, NULL);
   if (NULL == pSuite3) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the 3rd suite */
   if ((NULL == CU_add_test(pSuite3, "Test Read of Type ASCII", testReadAscii )) ||
   		(NULL == CU_add_test(pSuite3, "Test Read of Type Binary", testReadBin )))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

/* add a suite to the registry */
   pSuite4 = CU_add_suite("Test Extract Relation", NULL, NULL);
   if (NULL == pSuite4) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite4, "Test Values", testExtractRelation)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();

   CU_cleanup_registry();
   return CU_get_error();

}