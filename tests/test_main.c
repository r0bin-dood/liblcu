#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "tests.h"

int main(void)
{
    CU_initialize_registry();

    CU_pSuite list_suite = CU_add_suite("lcu_list", NULL, NULL);
    add_list_tests(list_suite);

    CU_pSuite tp_suite = CU_add_suite("lcu_tpool", NULL, NULL);
    add_tpool_tests(tp_suite);

    CU_pSuite logger_suite = CU_add_suite("lcu_logger", NULL, NULL);
    add_logger_tests(logger_suite);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}