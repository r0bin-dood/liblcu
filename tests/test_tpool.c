#include <CUnit/CUnit.h>
#include "tests.h"
#include "lcu_tpool.h"
#include "lcu_sync.h"

extern int force_malloc_failure;

#define NUM_THREADS 4

typedef struct w_data {
    lcu_sync_t w_sync;
    int w_id;
    void *w_data;
} w_data_t;

void test_worker(void *arg)
{
    w_data_t *w_data = (w_data_t *)arg;

    char buffer[12];
    sprintf(buffer, "%d", w_data->w_id);

    CU_ASSERT(strcmp((char *)w_data->w_data, buffer) == 0);

    lcu_sync_wait(&w_data->w_sync);
}

void test_tpool_create_destroy(void)
{
    lcu_tpool_t tp = NULL;
    CU_ASSERT(tp == NULL);
    // Creating 0 worker threads should fail
    tp = lcu_tpool_create(0);
    CU_ASSERT(tp == NULL);

    // Test destroy when tp is NULL
    lcu_tpool_destroy(&tp);
    CU_ASSERT(tp == NULL);

    // Force malloc failure
    force_malloc_failure = 1;
    tp = lcu_tpool_create(NUM_THREADS);
    force_malloc_failure = 0;
    CU_ASSERT(tp == NULL);

    tp = lcu_tpool_create(NUM_THREADS);
    CU_ASSERT(tp != NULL);

    // Check get size with NULL handle
    int total = 0;
    total = lcu_tpool_get_total_size(NULL);
    CU_ASSERT(total == 0);
    int avail = 0;
    avail = lcu_tpool_get_available_size(NULL);
    CU_ASSERT(avail == 0);

    // Check for correct sizes
    total = lcu_tpool_get_total_size(tp);
    CU_ASSERT(total == NUM_THREADS);
    avail = lcu_tpool_get_available_size(tp);
    CU_ASSERT(avail == NUM_THREADS);

    // Test destroy with NULL
    lcu_tpool_destroy(NULL);
    CU_ASSERT(tp != NULL);

    lcu_tpool_destroy(&tp);
    CU_ASSERT(tp == NULL);
}

void test_tpool_do_work(void)
{
    lcu_tpool_t tp = NULL;
    CU_ASSERT(tp == NULL);
    tp = lcu_tpool_create(NUM_THREADS);
    CU_ASSERT(tp != NULL);

    // Check for correct sizes
    int total = lcu_tpool_get_total_size(tp);
    CU_ASSERT(total == NUM_THREADS);
    int avail = lcu_tpool_get_available_size(tp);
    CU_ASSERT(avail == NUM_THREADS);

    int ret = 0;

    // Test do work with NULL handle
    ret = lcu_tpool_do_work(NULL, &test_worker, NULL);
    CU_ASSERT(ret == -1);
    CU_ASSERT(total == avail);

    // Test do work with NULL func
    ret = lcu_tpool_do_work(tp, NULL, NULL);
    CU_ASSERT(ret == -1);
    CU_ASSERT(total == avail);

    w_data_t w_data[NUM_THREADS];
    char *w_str[NUM_THREADS] = {
        "0",
        "1",
        "2",
        "3"
    };
    // Use all workers
    for (int i = 0; i < NUM_THREADS; i++)
    {
        w_data[i].w_id = i;
        w_data[i].w_data = w_str[i];
        lcu_sync_create(&w_data[i].w_sync);

        ret = lcu_tpool_do_work(tp, &test_worker, &w_data[i]);
        CU_ASSERT(ret == 0);
        CU_ASSERT((total - (i + 1)) == lcu_tpool_get_available_size(tp));
    }
    
    // No more workers
    ret = lcu_tpool_do_work(tp, &test_worker, NULL);
    CU_ASSERT(ret == -1);

    // Began waking and joining back workers
    for (int i = 0; i < NUM_THREADS; i++)
    {
        lcu_sync_signal(&w_data[i].w_sync);
        lcu_sync_destroy(&w_data[i].w_sync);
        usleep(10000);
        CU_ASSERT((i + 1) == lcu_tpool_get_available_size(tp));
    }

    CU_ASSERT(lcu_tpool_get_total_size(tp) == lcu_tpool_get_available_size(tp));

    lcu_tpool_destroy(&tp);
    CU_ASSERT(tp == NULL);
}

void add_tpool_tests(CU_pSuite suite)
{
    CU_add_test(suite, "test_tpool_create_destroy", test_tpool_create_destroy);
    CU_add_test(suite, "test_tpool_do_work", test_tpool_do_work);
}