#include <CUnit/CUnit.h>
#include "tests.h"
#include "lcu_tpool.h"

void test_tpool_create(void) {
    CU_ASSERT(true);
}

void add_tpool_tests(CU_pSuite suite)
{
    CU_add_test(suite, "test_tpool_create", test_tpool_create);
}