#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/Automated.h>
#include <CUnit/CUnit.h>
#include "CUnit/Basic.h"

#include "basicStructs.h"
#include "viceFunctions.h"

relation *r1,*r2,*r3;

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
	r1 = malloc(sizeof(relation));
	r1->tuples = malloc(10000 * sizeof(tuple));
	r1->size = 10000;
	for (uint32_t i=0; i<10000; i++)
	{
		if (i%10==0) r1->tuples[i].key=0;
		else r1->tuples[i].key=i;
	}
	r2 = malloc(sizeof(relation));
	r2->tuples = malloc(100000 * sizeof(tuple));
	r2->size = 100000;
	for (uint32_t i=0; i<100000; i++)
	{
		if (i%10==0) r2->tuples[i].key=0;
		else r2->tuples[i].key=i;
	}
	r3 = malloc(sizeof(relation));
	r3->tuples = malloc(10000000 * sizeof(tuple));
	r3->size = 10000000;
	for (uint32_t i=0; i<10000000; i++)
	{
		if (i%10==0) r3->tuples[i].key=0;
		else r3->tuples[i].key=i;
	}
	return 0;
}

int freeIdentical()
{
	free(r1->tuples);
	free(r1);
	free(r2->tuples);
	free(r2);
	free(r3->tuples);
	free(r3);
	return 0;
}

void Cloning()
{
	uint32_t successes = 0;
	printf("%f\n", IdenticalityTest(r1));
	for (uint32_t i=0; i<100; i++)
	{
		if (IdenticalityTest(r1) >= 0.09 && IdenticalityTest(r1) <= 0.11)
		{
			successes++;
		}
	}
	CU_ASSERT(successes>=50);
	CU_ASSERT(IdenticalityTest(r2) >= 0.09 && IdenticalityTest(r2) <= 0.11);
	CU_ASSERT(IdenticalityTest(r3) >= 0.09 && IdenticalityTest(r3) <= 0.11);
}

int main(void) {

	CU_pSuite pSuite1 = NULL;
	CU_pSuite pSuite2 = NULL;


	//Initialize the CUnit test registry
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

  /* add a suite to the registry */
   pSuite1 = CU_add_suite("Power Test", NULL, NULL);
   if (NULL == pSuite1) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite2 = CU_add_suite("Eye Test", initIdentical, freeIdentical);
   if (NULL == pSuite2) {
      CU_cleanup_registry();
      return CU_get_error();
   }


   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite1, "Test Values", Powerful)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite2, "Test Values", Cloning)))
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
