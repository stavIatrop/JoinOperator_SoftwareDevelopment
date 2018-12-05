#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "basicStructs.h"
#include "viceFunctions.h"
#include "resultListInterface.h"
#include "chainFollowerInterface.h"
#include "sideFunctions.h"
#include "hashing.h"
#include "indexManip.h"

#include <CUnit/Automated.h>
#include <CUnit/CUnit.h>
#include "CUnit/Basic.h"

relation *r, *s;
reorderedR *R, *S;
headResult * list;
indexArray * indexA;
myint_t hash1=0, size = 10000000, size2 = 2000000;


// !! WHOLE FILE DEPRECATED, does not work !!


int initialiseStructs(void) {
        r = (relation *) malloc(sizeof(relation));
        s = (relation *) malloc(sizeof(relation));

	r->tuples = malloc(size * sizeof(tuple));
        s->tuples = malloc(size2 * sizeof(tuple));
	r->size=size;
        s->size=size2;

        //myint_t random;
	//srand(time(NULL));

	for (myint_t i=0; i<size; i++)
        {
                //random = rand();
                r->tuples[i].key = i;
                r->tuples[i].payload = i;
        }

        for (myint_t i=0; i<size2; i++)
        {
                //random = rand();
                s->tuples[i].key = i;
                s->tuples[i].payload = i;
        }

        
        R = reorderRelation(r,&hash1);
        S = reorderRelation(s,&hash1);

        indexA = indexing(R, hash1, 101);

        list = search(indexA, S, 101);

        return 0;
}

void testResults() {
        CU_ASSERT(checkResults(list) == 0);
}

int freeStructs() {
        freeResultList(list);
        freeIndexArray(indexA);
        
        free(R->rel->tuples);
	free(S->rel->tuples);
        free(R->rel);
        free(S->rel);
        free(R);
        free(S);

        return 0;
}

int main(void) {

	CU_pSuite pSuite = NULL;


	//Initialize the CUnit test registry
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

  /* add a suite to the registry */
   pSuite = CU_add_suite("Main Test", initialiseStructs, freeStructs);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "Check matching rowIds", testResults)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   // CU_set_output_filename("Chain_Test");
   // CU_automated_run_tests();
   // CU_list_tests_to_file();

   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();

   CU_cleanup_registry();
   return CU_get_error();

}

/*int main(int argc, char const *argv[])
{
        myint_t hash1=0, size = 10000000, size2 = 20000000;
	relation *r, *s;
        r = (relation *) malloc(sizeof(relation));
        s = (relation *) malloc(sizeof(relation));

	r->tuples = malloc(size * sizeof(tuple));
        s->tuples = malloc(size2 * sizeof(tuple));
	r->size=size;
        s->size=size2;
	myint_t random;
	srand(time(NULL));
	reorderedR *R, *S;

	for (myint_t i=0; i<size; i++)
        {
                random = rand();
                r->tuples[i].key = i;
                r->tuples[i].payload = i;
        }
        printf("Entering reordering function\n");
        R = reorderRelation(r,&hash1);
        printf("Exited reordering function\n");

        for (myint_t i=0; i<size2; i++)
        {
                random = rand();
                s->tuples[i].key = i;
                s->tuples[i].payload = i;
        }
        printf("Entering reordering function\n");
        S = reorderRelation(s,&hash1);
        printf("Exited reordering function\n");

        printf("Starting indexing\n");
        indexArray * indexA = indexing(R, hash1, 101);

        printf("Starting searching\n");
        headResult * list = search(*indexA, S, 101);
        printf("Exiting searching Nodes = %ld | ResultsPerNode = %ld\n", list->numbOfNodes, list->firstNode->size);
        //for(int j = 0; j < list->firstNode->size; j++) {
        //        printf("Row1 = %ld | Row2 = %ld\n", list->firstNode->tuples[j].rowR, list->firstNode->tuples[j].rowS);
        //}

        int result = checkResults(list);

        freeResultList(list);
        freeIndexArray(indexA);
        
        free(R->rel->tuples);
	free(S->rel->tuples);
        free(R->rel);
        free(S->rel);
        free(R);
        free(S);

        if(result == 0) {
                return 0;
        }
        else {
                return -1;
        }

        return 0;
}*/
