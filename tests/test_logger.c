#include <CUnit/CUnit.h>
#include "tests.h"
#include "lcu_logger.h"

extern int force_malloc_failure;

void test_logger_create_destroy(void)
{
    // Call destroy before calling create
    lcu_logger_destroy();

    int ret = 0;
    ret = lcu_logger_create(NULL);
    CU_ASSERT(ret == -1);

    force_malloc_failure = 1;
    ret = lcu_logger_create(LCU_STDOUT);
    force_malloc_failure = 0;
    CU_ASSERT(ret == -1);

    char *test_file = "./tests/out/test_logger_create_destroy.txt";
    ret = lcu_logger_create(test_file);
    CU_ASSERT(ret == 0);

    // Testing print
    lcu_logger_print("From logger test\n");
    usleep(50000);

    lcu_logger_destroy();
}

void test_logger_print(void)
{
    char *test_file = "./tests/out/test_logger_print.txt";
    int ret = lcu_logger_create(test_file);
    CU_ASSERT(ret == 0);

    // Print NULL
    lcu_logger_print(NULL);

    // Testing print
    lcu_logger_print("From logger test\n");
    usleep(50000);

    lcu_logger_destroy();
}

void add_logger_tests(CU_pSuite suite)
{
    CU_add_test(suite, "test_logger_create_destroy", test_logger_create_destroy);
    CU_add_test(suite, "test_logger_print", test_logger_print);
}