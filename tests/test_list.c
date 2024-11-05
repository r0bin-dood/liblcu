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
    ll = lcu_list_create();
    force_malloc_failure = 0;
    CU_ASSERT(ll == NULL);

    // Destroy when ll is NULL
    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);

    // Setting a cleanup callback to test proper destruction
    ll = lcu_list_create();
    CU_ASSERT(ll != NULL);

    // Using insert to test destruction
    int val = 1337;
    int ret = 0;
    ret = lcu_list_insert_front(ll, &val, &cleanup_func);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val == *((int *)lcu_list_peek_front(ll)));

    // Destroy list, cleanup callback should be called
    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
    CU_ASSERT(val == CLEANED);

    // Create list again, with no callback
    ll = lcu_list_create();
    CU_ASSERT(ll != NULL);

    // Using insert to test destruction
    val = 1337;
    ret = 0;
    ret = lcu_list_insert_front(ll, &val, NULL);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val == *((int *)lcu_list_peek_front(ll)));

    // Destroy NULL
    lcu_list_destroy(NULL);
    CU_ASSERT(ll != NULL);

    // Destroy list, val should be left as is
    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
    CU_ASSERT(val == 1337);
}

void test_list_insert_front(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create();
    CU_ASSERT(ll != NULL);

    int val = 1337;
    int ret = 0;

    // NULL as handle
    ret = lcu_list_insert_front(NULL, &val, NULL);
    CU_ASSERT(ret == -1);
    
    // Force malloc failure
    force_malloc_failure = 1;
    ret = lcu_list_insert_front(ll, &val, NULL);
    force_malloc_failure = 0;
    CU_ASSERT(ret == -1);

    // Insert value
    ret = lcu_list_insert_front(ll, &val, NULL);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val == *((int *)lcu_list_peek_front(ll)));

    val = 8888;
    ret = lcu_list_insert_front(ll, &val, NULL);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val == *((int *)lcu_list_peek_front(ll)));

    ret = lcu_list_insert_front(ll, NULL, NULL);
    CU_ASSERT(ret == 0);
    CU_ASSERT(NULL == lcu_list_peek_front(ll));

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void test_list_insert_back(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create();
    CU_ASSERT(ll != NULL);

    int val = 1337;
    int ret = 0;

    // NULL as handle
    ret = lcu_list_insert_back(NULL, &val, NULL);
    CU_ASSERT(ret == -1);
    
    // Force malloc failure
    force_malloc_failure = 1;
    ret = lcu_list_insert_back(ll, &val, NULL);
    force_malloc_failure = 0;
    CU_ASSERT(ret == -1);

    // Insert value
    ret = lcu_list_insert_back(ll, &val, NULL);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val == *((int *)lcu_list_peek_back(ll)));

    val = 8888;
    ret = lcu_list_insert_back(ll, &val, NULL);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val == *((int *)lcu_list_peek_back(ll)));

    ret = lcu_list_insert_back(ll, NULL, NULL);
    CU_ASSERT(ret == 0);
    CU_ASSERT(NULL == lcu_list_peek_back(ll));

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void test_list_insert_at_i(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create();
    CU_ASSERT(ll != NULL);

    int val = 1337;
    int ret = 0;

    // NULL as handle
    ret = lcu_list_insert(NULL, 0, &val, NULL);
    CU_ASSERT(ret == -1);
    
    // Force malloc failure
    force_malloc_failure = 1;
    ret = lcu_list_insert(ll, 0, &val, NULL);
    force_malloc_failure = 0;
    CU_ASSERT(ret == -1);

    // Insert value
    ret = lcu_list_insert(ll, 0, &val, NULL);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val == *((int *)lcu_list_peek(ll, 0)));

    int val7 = 8888;
    ret = lcu_list_insert(ll, 0, &val7, NULL);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val7 == *((int *)lcu_list_peek(ll, 0)));

    // Inserting at the back
    int val2 = 9999;
    ret = lcu_list_insert(ll, lcu_list_get_size(ll) - 1, &val2, NULL);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val2 == *((int *)lcu_list_peek_back(ll)));

    // Inserting at the front
    int val3 = 7777;
    ret = lcu_list_insert(ll, 0, &val3, NULL);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val3 == *((int *)lcu_list_peek_front(ll)));

    // Inserting at the middle
    int val4 = 1111;
    int middle = lcu_list_get_size(ll) / 2;
    ret = lcu_list_insert(ll, middle, &val4, NULL);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val4 == *((int *)lcu_list_peek(ll, middle)));

    // Inserting at out of bounds should fail
    int val5 = 2222;
    int pos = 496;
    ret = lcu_list_insert(ll, pos, &val5, NULL);
    CU_ASSERT(ret == -1);

    // Inserting at a negative should fail
    int val6 = 3333;
    pos = -13;
    ret = lcu_list_insert(ll, pos, &val6, NULL);
    CU_ASSERT(ret == -1);

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

#define NUM 6

void test_list_remove_front(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create();
    CU_ASSERT(ll != NULL);

    int val[NUM] = {10,11,12,13,14,15};
    int ret = 0;

    // Remove from empty list
    ret = lcu_list_remove_front(ll);
    CU_ASSERT(ret == -1);

    // Insert values to remove
    for (int i = 0; i < NUM; i++)
    {
        ret = lcu_list_insert_back(ll, &val[i], NULL);
        CU_ASSERT(ret == 0);
        CU_ASSERT(val[i] == *((int *)lcu_list_peek_back(ll)));
    }
    // Check first item
    CU_ASSERT(val[0] == *((int *)lcu_list_peek_front(ll)));

    // NULL as handle
    ret = lcu_list_remove_front(NULL);
    CU_ASSERT(ret == -1);
    CU_ASSERT(val[0] == *((int *)lcu_list_peek_front(ll)));

    ret = lcu_list_remove_front(ll);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val[1] == *((int *)lcu_list_peek_front(ll)));

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void test_list_remove_back(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create();
    CU_ASSERT(ll != NULL);

    int val[NUM] = {10,11,12,13,14,15};
    int ret = 0;

    // Remove from empty list
    ret = lcu_list_remove_back(ll);
    CU_ASSERT(ret == -1);

    // Insert values to remove
    for (int i = 0; i < NUM; i++)
    {
        ret = lcu_list_insert_back(ll, &val[i], NULL);
        CU_ASSERT(ret == 0);
        CU_ASSERT(val[i] == *((int *)lcu_list_peek_back(ll)));
    }
    // Check last item
    CU_ASSERT(val[NUM - 1] == *((int *)lcu_list_peek_back(ll)));

    // NULL as handle
    ret = lcu_list_remove_back(NULL);
    CU_ASSERT(ret == -1);
    CU_ASSERT(val[NUM - 1] == *((int *)lcu_list_peek_back(ll)));

    ret = lcu_list_remove_back(ll);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val[NUM - 2] == *((int *)lcu_list_peek_back(ll)));

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void test_list_remove_at_i(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create();
    CU_ASSERT(ll != NULL);

    int val[NUM] = {10,11,12,13,14,15};
    int ret = 0;

    // Remove from empty list
    ret = lcu_list_remove(ll, 0);
    CU_ASSERT(ret == -1);

    // Insert values to remove
    for (int i = 0; i < NUM; i++)
    {
        ret = lcu_list_insert_back(ll, &val[i], NULL);
        CU_ASSERT(ret == 0);
        CU_ASSERT(val[i] == *((int *)lcu_list_peek_back(ll)));
    }

    // NULL as handle
    ret = lcu_list_remove(NULL, 0);
    CU_ASSERT(ret == -1);
    CU_ASSERT(val[0] == *((int *)lcu_list_peek(ll, 0)));

    // Remove at out of bounds should fail
    int pos = 496;
    ret = lcu_list_remove(ll, pos);
    CU_ASSERT(ret == -1);

    // Remove at negative should remove from back
    pos = -13;
    ret = lcu_list_remove(ll, pos);
    CU_ASSERT(ret == -1);

    // Remove from middle
    pos = lcu_list_get_size(ll) / 2;
    ret = lcu_list_remove(ll, pos);
    CU_ASSERT(ret == 0);
    CU_ASSERT(val[(NUM - 1) / 2] == *((int *)lcu_list_peek(ll, pos)));
    
    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void add_list_tests(CU_pSuite suite)
{
    CU_add_test(suite, "test_list_create_destroy", test_list_create_destroy);
    CU_add_test(suite, "test_list_insert_front", test_list_insert_front);
    CU_add_test(suite, "test_list_insert_back", test_list_insert_back);
    CU_add_test(suite, "test_list_insert_at_i", test_list_insert_at_i);
    CU_add_test(suite, "test_list_remove_front", test_list_remove_front);
    CU_add_test(suite, "test_list_remove_back", test_list_remove_back);
    CU_add_test(suite, "test_list_remove_at_i", test_list_remove_at_i);
}