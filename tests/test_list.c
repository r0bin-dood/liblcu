#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "tests.h"
#include "lcu_list.h"

#define CLEANED 1996

extern int force_malloc_failure;

void cleanup_func(void *arg)
{
    *((int *)arg) = CLEANED;
}

void test_list_create_destroy()
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);

    // Force a malloc failure
    force_malloc_failure = 1;
    ll = lcu_list_create(NULL);
    force_malloc_failure = 0;
    CU_ASSERT(ll == NULL);

    // Setting a cleanup callback to test proper destruction
    ll = lcu_list_create(&cleanup_func);
    CU_ASSERT(ll != NULL);

    // Using insert to test destruction
    int val = 1337;
    int ret = 0;
    ret = lcu_list_insert_front(ll, &val);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val == *((int *)lcu_list_peek_front(ll)));

    // Destroy list, cleanup callback should be called
    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
    CU_ASSERT(val == CLEANED);

    // Create list again, with no callback
    ll = lcu_list_create(NULL);
    CU_ASSERT(ll != NULL);

    // Using insert to test destruction
    val = 1337;
    ret = 0;
    ret = lcu_list_insert_front(ll, &val);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val == *((int *)lcu_list_peek_front(ll)));

    // Destroy list, val should be left as is
    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
    CU_ASSERT(val == 1337);
}

void test_list_insert_front(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create(NULL);
    CU_ASSERT(ll != NULL);

    int val = 1337;
    int ret = 0;

    // NULL as handle
    ret = lcu_list_insert_front(NULL, &val);
    CU_ASSERT(ret == -1);
    
    // Force malloc failure
    force_malloc_failure = 1;
    ret = lcu_list_insert_front(ll, &val);
    force_malloc_failure = 0;
    CU_ASSERT(ret == -1);

    // Insert value
    ret = lcu_list_insert_front(ll, &val);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val == *((int *)lcu_list_peek_front(ll)));

    val = 8888;
    ret = lcu_list_insert_front(ll, &val);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val == *((int *)lcu_list_peek_front(ll)));

    ret = lcu_list_insert_front(ll, NULL);
    CU_ASSERT(ret == 0);
    CU_ASSERT(NULL == lcu_list_peek_front(ll));

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void test_list_insert_back(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create(NULL);
    CU_ASSERT(ll != NULL);

    int val = 1337;
    int ret = 0;

    // NULL as handle
    ret = lcu_list_insert_front(NULL, &val);
    CU_ASSERT(ret == -1);
    
    // Force malloc failure
    force_malloc_failure = 1;
    ret = lcu_list_insert_front(ll, &val);
    force_malloc_failure = 0;
    CU_ASSERT(ret == -1);

    // Insert value
    ret = lcu_list_insert_front(ll, &val);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val == *((int *)lcu_list_peek_front(ll)));

    val = 8888;
    ret = lcu_list_insert_front(ll, &val);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val == *((int *)lcu_list_peek_front(ll)));

    ret = lcu_list_insert_front(ll, NULL);
    CU_ASSERT(ret == 0);
    CU_ASSERT(NULL == lcu_list_peek_front(ll));

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void add_list_tests(CU_pSuite suite)
{
    CU_add_test(suite, "test_list_create_destroy", test_list_create_destroy);
    CU_add_test(suite, "test_list_insert_front", test_list_insert_front);
    CU_add_test(suite, "test_list_insert_back", test_list_insert_back);
}