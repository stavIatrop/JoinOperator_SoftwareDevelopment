#include <stdlib.h>
#include "queryStructs.h"
#include "jointPerformer.h"
#include <CUnit/Automated.h>
#include <CUnit/CUnit.h>
#include "CUnit/Basic.h"

myint_t *arr1D;

myint_t * create_1DArray(int rows, int value) {
    myint_t * arr = (myint_t *) malloc(rows * sizeof(myint_t));
    for(int whichRow = 0; whichRow < rows; whichRow++) {
        arr[whichRow] = value;
    }
    return arr;
}


int initArray()
{
	arr1D = create_1DArray(20,3);
	arr1D[5]=7;
}

int freeArray()
{
	free(arr1D);
}

void testNextRowId()
{
	myint_t i=0;
	CU_ASSERT(findNextRowId(arr1D,i,20)==1);
	CU_ASSERT(findNextRowId(arr1D,i,20)==5);
	CU_ASSERT(findNextRowId(arr1D,i,20)==6);
	CU_ASSERT(findNextRowId(arr1D,i,20)==20);
}

int main(void)
{
        CU_pSuite pSuite1 = NULL;

	pSuite1 = CU_add_suite("Test Next Row Id", initArray, freeArray);
	if (NULL == pSuite1) {
   		CU_cleanup_registry();
   	   	return CU_get_error();
   	}
	if (NULL == CU_add_test(pSuite1, "Test it", testNextRowId))
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
