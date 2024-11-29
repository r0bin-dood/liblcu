#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "tests.h"
#include "lcu_list.h"

#define CLEANED 1996

void cleanup_func(void *arg)
{
    *((int *)arg) = CLEANED;
}

void test_list_create_destroy()
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);

    // Destroy when ll is NULL
    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);

    ll = lcu_list_create(32);
    CU_ASSERT(ll != NULL);

    // Using insert to test destruction
    int val = 1337;
    int ret = 0;
    ret = lcu_list_insert_front(ll, &val, &cleanup_func);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val == *((int *)lcu_list_peek_front(ll)));

    // Destroy list, cleanup callback should be called
    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
    CU_ASSERT(val == CLEANED);

    ll = lcu_list_create(32);
    CU_ASSERT(ll != NULL);

    // Using insert to test destruction
    val = 1337;
    ret = 0;
    ret = lcu_list_insert_front(ll, &val, NULL);
    CU_ASSERT(ret == LCU_ERR_OK);
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
    ll = lcu_list_create(32);
    CU_ASSERT(ll != NULL);

    int val = 1337;
    int ret = 0;

    // NULL as handle
    ret = lcu_list_insert_front(NULL, &val, NULL);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Insert value
    ret = lcu_list_insert_front(ll, &val, NULL);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val == *((int *)lcu_list_peek_front(ll)));

    val = 8888;
    ret = lcu_list_insert_front(ll, &val, NULL);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val == *((int *)lcu_list_peek_front(ll)));

    ret = lcu_list_insert_front(ll, NULL, NULL);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(NULL == lcu_list_peek_front(ll));

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void test_list_insert_back(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create(32);
    CU_ASSERT(ll != NULL);

    int val = 1337;
    int ret = 0;

    // NULL as handle
    ret = lcu_list_insert_back(NULL, &val, NULL);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Insert value
    ret = lcu_list_insert_back(ll, &val, NULL);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val == *((int *)lcu_list_peek_back(ll)));

    val = 8888;
    ret = lcu_list_insert_back(ll, &val, NULL);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val == *((int *)lcu_list_peek_back(ll)));

    ret = lcu_list_insert_back(ll, NULL, NULL);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(NULL == lcu_list_peek_back(ll));

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void test_list_insert(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create(32);
    CU_ASSERT(ll != NULL);

    int val = 1337;
    int ret = 0;

    // NULL as handle
    ret = lcu_list_insert(NULL, 0, &val, NULL);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Insert value
    ret = lcu_list_insert(ll, 0, &val, NULL);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val == *((int *)lcu_list_peek(ll, 0)));

    int val7 = 8888;
    ret = lcu_list_insert(ll, 0, &val7, NULL);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val7 == *((int *)lcu_list_peek(ll, 0)));

    // Inserting at the back
    int val2 = 9999;
    ret = lcu_list_insert(ll, lcu_list_get_size(ll), &val2, NULL);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val2 == *((int *)lcu_list_peek_back(ll)));

    // Inserting at the front
    int val3 = 7777;
    ret = lcu_list_insert(ll, 0, &val3, NULL);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val3 == *((int *)lcu_list_peek_front(ll)));

    // Inserting at the middle
    int val4 = 1111;
    int middle = lcu_list_get_size(ll) / 2;
    ret = lcu_list_insert(ll, middle, &val4, NULL);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val4 == *((int *)lcu_list_peek(ll, middle)));

    // Inserting at out of bounds should fail
    int val5 = 2222;
    int pos = 496;
    ret = lcu_list_insert(ll, pos, &val5, NULL);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Inserting at a negative should fail
    int val6 = 3333;
    pos = -13;
    ret = lcu_list_insert(ll, pos, &val6, NULL);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void test_list_move_to_front(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create(32);
    CU_ASSERT(ll != NULL);

    int val[32] = {1111, 2222, 3333, 4444};

    // move on empty list should fail
    int ret = lcu_list_move(ll, 0, 1);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Insert value
    for (int i = 0; i < 4; i++) {
        ret = lcu_list_insert(ll, i, &val[i], NULL);
        CU_ASSERT(ret == LCU_ERR_OK);
        CU_ASSERT(val[i] == *((int *)lcu_list_peek(ll, i)));
    }

    // NULL handle should fail
    ret = lcu_list_move_to_front(NULL, 1);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Negative should fail
    ret = lcu_list_move_to_front(ll, -1);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // out of bounds should fail
    ret = lcu_list_move_to_front(ll, 43);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // No effect
    ret = lcu_list_move_to_front(ll, 0);
    CU_ASSERT(ret == LCU_ERR_OK);

    ret = lcu_list_move_to_front(ll, 1);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val[1] == *((int *)lcu_list_peek_front(ll)));
    CU_ASSERT(val[3] == *((int *)lcu_list_peek_back(ll)));

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void test_list_move_to_back(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create(32);
    CU_ASSERT(ll != NULL);

    int val[32] = {1111, 2222, 3333, 4444};

    // move on empty list should fail
    int ret = lcu_list_move(ll, 0, 1);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Insert value
    for (int i = 0; i < 4; i++) {
        ret = lcu_list_insert(ll, i, &val[i], NULL);
        CU_ASSERT(ret == LCU_ERR_OK);
        CU_ASSERT(val[i] == *((int *)lcu_list_peek(ll, i)));
    }

    // NULL handle should fail
    ret = lcu_list_move_to_back(NULL, 0);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Negative should fail
    ret = lcu_list_move_to_back(ll, -1);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // out of bounds should fail
    ret = lcu_list_move_to_back(ll, 43);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // No effect
    ret = lcu_list_move_to_back(ll, lcu_list_get_size(ll) - 1);
    CU_ASSERT(ret == LCU_ERR_OK);

    ret = lcu_list_move_to_back(ll, 0);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val[1] == *((int *)lcu_list_peek_front(ll)));
    CU_ASSERT(val[0] == *((int *)lcu_list_peek_back(ll)));

    ret = lcu_list_move_to_back(ll, 1);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val[3] == *((int *)lcu_list_peek(ll, 1)));
    CU_ASSERT(val[2] == *((int *)lcu_list_peek_back(ll)));

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void test_list_move(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create(32);
    CU_ASSERT(ll != NULL);

    int val[32] = {1111, 2222, 3333, 4444, 5555, 6666};

    // move on empty list should fail
    int ret = lcu_list_move(ll, 0, 1);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Insert value
    for (int i = 0; i < 6; i++) {
        ret = lcu_list_insert(ll, i, &val[i], NULL);
        CU_ASSERT(ret == LCU_ERR_OK);
        CU_ASSERT(val[i] == *((int *)lcu_list_peek(ll, i)));
    }

    // NULL handle should fail
    ret = lcu_list_move(NULL, 0, 2);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Negative should fail
    ret = lcu_list_move(ll, -1, 1);
    CU_ASSERT(ret == LCU_ERR_INVAL);
    ret = lcu_list_move(ll, 1, -1);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // out of bounds should fail
    ret = lcu_list_move(ll, 43, 1);
    CU_ASSERT(ret == LCU_ERR_INVAL);
    ret = lcu_list_move(ll, 1, 43);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // No effect
    ret = lcu_list_move(ll, 1, 1);
    CU_ASSERT(ret == LCU_ERR_OK);

    // Moving node from front to back
    ret = lcu_list_move(ll, 0, lcu_list_get_size(ll) - 1);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val[1] == *((int *)lcu_list_peek_front(ll)));
    CU_ASSERT(val[0] == *((int *)lcu_list_peek_back(ll)));
    // Moving node to immediately adjacent node is basically a swap operation
    ret = lcu_list_move(ll, 1, 2);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val[3] == *((int *)lcu_list_peek(ll, 1)));
    CU_ASSERT(val[2] == *((int *)lcu_list_peek(ll, 2)));
    ret = lcu_list_move(ll, 2, 1);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val[2] == *((int *)lcu_list_peek(ll, 1)));
    CU_ASSERT(val[3] == *((int *)lcu_list_peek(ll, 2)));

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void test_list_swap_with_front(void)
{
    CU_ASSERT(true);
}

void test_list_swap_with_back(void)
{
    CU_ASSERT(true);
}

void test_list_swap(void)
{
    CU_ASSERT(true);
}

#define NUM 6

void test_list_remove_front(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create(32);
    CU_ASSERT(ll != NULL);

    int val[NUM] = {10,11,12,13,14,15};
    int ret = 0;

    // Remove from empty list
    ret = lcu_list_remove_front(ll);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Insert values to remove
    for (int i = 0; i < NUM; i++)
    {
        ret = lcu_list_insert_back(ll, &val[i], NULL);
        CU_ASSERT(ret == LCU_ERR_OK);
        CU_ASSERT(val[i] == *((int *)lcu_list_peek_back(ll)));
    }
    // Check first item
    CU_ASSERT(val[0] == *((int *)lcu_list_peek_front(ll)));

    // NULL as handle
    ret = lcu_list_remove_front(NULL);
    CU_ASSERT(ret == LCU_ERR_INVAL);
    CU_ASSERT(val[0] == *((int *)lcu_list_peek_front(ll)));

    ret = lcu_list_remove_front(ll);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val[1] == *((int *)lcu_list_peek_front(ll)));

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void test_list_remove_back(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create(32);
    CU_ASSERT(ll != NULL);

    int val[NUM] = {10,11,12,13,14,15};
    int ret = 0;

    // Remove from empty list
    ret = lcu_list_remove_back(ll);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Insert values to remove
    for (int i = 0; i < NUM; i++)
    {
        ret = lcu_list_insert_back(ll, &val[i], NULL);
        CU_ASSERT(ret == LCU_ERR_OK);
        CU_ASSERT(val[i] == *((int *)lcu_list_peek_back(ll)));
    }
    // Check last item
    CU_ASSERT(val[NUM - 1] == *((int *)lcu_list_peek_back(ll)));

    // NULL as handle
    ret = lcu_list_remove_back(NULL);
    CU_ASSERT(ret == LCU_ERR_INVAL);
    CU_ASSERT(val[NUM - 1] == *((int *)lcu_list_peek_back(ll)));

    ret = lcu_list_remove_back(ll);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val[NUM - 2] == *((int *)lcu_list_peek_back(ll)));

    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

void test_list_remove(void)
{
    lcu_list_t ll = NULL;
    CU_ASSERT(ll == NULL);
    ll = lcu_list_create(32);
    CU_ASSERT(ll != NULL);

    int val[NUM] = {10,11,12,13,14,15};
    int ret = 0;

    // Remove from empty list
    ret = lcu_list_remove(ll, 0);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Insert values to remove
    for (int i = 0; i < NUM; i++)
    {
        ret = lcu_list_insert_back(ll, &val[i], NULL);
        CU_ASSERT(ret == LCU_ERR_OK);
        CU_ASSERT(val[i] == *((int *)lcu_list_peek_back(ll)));
    }

    // NULL as handle
    ret = lcu_list_remove(NULL, 0);
    CU_ASSERT(ret == LCU_ERR_INVAL);
    CU_ASSERT(val[0] == *((int *)lcu_list_peek(ll, 0)));

    // Remove at out of bounds should fail
    int pos = 496;
    ret = lcu_list_remove(ll, pos);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Remove at negative should remove from back
    pos = -13;
    ret = lcu_list_remove(ll, pos);
    CU_ASSERT(ret == LCU_ERR_INVAL);

    // Remove from middle
    pos = lcu_list_get_size(ll) / 2;
    ret = lcu_list_remove(ll, pos);
    CU_ASSERT(ret == LCU_ERR_OK);
    CU_ASSERT(val[(NUM - 1) / 2] == *((int *)lcu_list_peek(ll, pos)));
    
    lcu_list_destroy(&ll);
    CU_ASSERT(ll == NULL);
}

/*
lcu_list_t lcu_list_create();
size_t lcu_list_get_size(lcu_list_t handle);
int lcu_list_insert_front(lcu_list_t handle, void *value, lcu_generic_callback cleanup_func);
int lcu_list_insert_back(lcu_list_t handle, void *value, lcu_generic_callback cleanup_func);
int lcu_list_insert(lcu_list_t handle, int i, void *value, lcu_generic_callback cleanup_func);
void *lcu_list_peek_front(lcu_list_t handle);
void *lcu_list_peek_back(lcu_list_t handle);
void *lcu_list_peek(lcu_list_t handle, int i);
int lcu_list_move_to_front(lcu_list_t handle, int i);
int lcu_list_move_to_back(lcu_list_t handle, int i);
int lcu_list_move(lcu_list_t handle, int from, int to);
int lcu_list_swap_with_front(lcu_list_t handle, int i);
int lcu_list_swap_with_back(lcu_list_t handle, int i);
int lcu_list_swap(lcu_list_t handle, int i, int j);
int lcu_list_remove_front(lcu_list_t handle);
int lcu_list_remove_back(lcu_list_t handle);
int lcu_list_remove(lcu_list_t handle, int i);
void lcu_list_destroy(lcu_list_t *handle);
*/

void add_list_tests(CU_pSuite suite)
{
    CU_add_test(suite, "test_list_create_destroy", test_list_create_destroy);
    CU_add_test(suite, "test_list_insert_front", test_list_insert_front);
    CU_add_test(suite, "test_list_insert_back", test_list_insert_back);
    CU_add_test(suite, "test_list_insert", test_list_insert);
    CU_add_test(suite, "test_list_move_to_front", test_list_move_to_front);
    CU_add_test(suite, "test_list_move_to_back", test_list_move_to_back);
    CU_add_test(suite, "test_list_move", test_list_move);
    CU_add_test(suite, "test_list_swap_with_front", test_list_swap_with_front);
    CU_add_test(suite, "test_list_swap_with_back", test_list_swap_with_back);
    CU_add_test(suite, "test_list_swap", test_list_swap);
    CU_add_test(suite, "test_list_remove_front", test_list_remove_front);
    CU_add_test(suite, "test_list_remove_back", test_list_remove_back);
    CU_add_test(suite, "test_list_remove", test_list_remove);
}