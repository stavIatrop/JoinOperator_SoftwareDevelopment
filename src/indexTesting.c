#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <CUnit/Automated.h>
#include <CUnit/CUnit.h>


#include "sideFunctions.h"
#include "CUnit/Basic.h"


uint32_t * chain = NULL;
uint32_t * buckets = NULL;

int InitializeStructs() {

	chain = (uint32_t *) malloc(8 * sizeof(uint32_t));
	buckets = (uint32_t *) malloc(5 * sizeof(uint32_t));

	int i;
	for(i = 0; i < 8; i++)
		chain[i] = 0;
	for(i = 0; i < 5; i++)
		buckets[i] = 0;

	
	return 0;

}

int freeStructs() {

	free(chain);
	free(buckets);
	return 0;
}

void testUpdateChain() {

	
	updateChain(chain, buckets, 3, 2);
	updateChain(chain, buckets, 3, 3);
	updateChain(chain, buckets, 3, 6);

	CU_ASSERT(chain[2] == 0);
	CU_ASSERT(chain[3] == 3);
	CU_ASSERT(chain[6] == 4);
	CU_ASSERT(buckets[3] == 7);
	return;
}


int main(void) {

	CU_pSuite pSuite = NULL;

	//Initialize the CUnit test registry
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

  /* add a suite to the registry */
   pSuite = CU_add_suite("Chain Suite", InitializeStructs, freeStructs);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "Test updateChain", testUpdateChain)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   CU_set_output_filename("Chain_Test");
   CU_automated_run_tests();
   CU_list_tests_to_file();

   // CU_basic_set_mode(CU_BRM_VERBOSE);
   // CU_basic_run_tests();

   CU_cleanup_registry();
   return CU_get_error();

}