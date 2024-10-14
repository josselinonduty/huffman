#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdlib.h>

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

    pSuite = CU_add_suite("Compress", init_suite, clean_suite);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    pSuite = CU_add_suite("Decompress", init_suite, clean_suite);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
    printf("\n");

    if (CU_get_number_of_tests_failed() > 0)
    {
        CU_cleanup_registry();
        return EXIT_FAILURE;
    }

    CU_cleanup_registry();
    return EXIT_SUCCESS;
}