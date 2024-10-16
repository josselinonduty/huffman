#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdlib.h>

#include "statistics_test.h"

int init_suite(void)
{
	return 0;
}

int clean_suite(void)
{
	return 0;
}

int main(void)
{
	CU_pSuite pSuite = NULL;

	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	pSuite = CU_add_suite("Statistics", init_suite, clean_suite);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if (NULL ==
	    CU_add_test(pSuite, "test_frequencies_instance",
			test_frequencies_instance)
	    || NULL == CU_add_test(pSuite, "test_frequencies_get_set",
				   test_frequencies_get_set)
	    || NULL == CU_add_test(pSuite, "test_statistic_instance",
				   test_statistic_instance)
	    || NULL == CU_add_test(pSuite, "test_statistic_copy",
				   test_statistic_copy)
	    || NULL == CU_add_test(pSuite, "test_statistic_compare",
				   test_statistic_compare)) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_basic_show_failures(CU_get_failure_list());
	printf("\n");

	if (CU_get_number_of_tests_failed() > 0) {
		CU_cleanup_registry();
		return EXIT_FAILURE;
	}

	CU_cleanup_registry();
	return EXIT_SUCCESS;
}
