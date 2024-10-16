
#include <unistd.h>
#include "main.h"

#define NUM_THREADS 4

struct worker {
    int id;
    void *data;
};

static lcu_sync_t worker_sync;

void test_func(void *arg)
{
    int id = *((int *)arg);
    lcu_logger_print("   from %d: before\n", id);
    lcu_sync_wait(&worker_sync);
    lcu_logger_print("   from %d: after\n", id);
}

void example_tpool()
{
    lcu_logger_print("%s():\n", __func__);

    lcu_tpool_t tp = lcu_tpool_create(NUM_THREADS);
    lcu_sync_create(&worker_sync);

    lcu_logger_print("avail workers: %lu, total: %lu\n", lcu_tpool_get_available_size(tp), lcu_tpool_get_total_size(tp));

    int i_arr[NUM_THREADS] = {0,1,2,3};
    for (int i = 0; i < NUM_THREADS; i++)
        lcu_tpool_do_work(tp, &test_func, &i_arr[i]);
    int i = 4;
    if (lcu_tpool_do_work(tp, &test_func, &i) == -1)
        lcu_logger_print("Not enough workers!\n");
    lcu_logger_print("avail workers: %lu, total: %lu\n", lcu_tpool_get_available_size(tp), lcu_tpool_get_total_size(tp));
    usleep(10000);
    lcu_sync_signal_n(&worker_sync, NUM_THREADS);
    lcu_logger_print("avail workers: %lu, total: %lu\n", lcu_tpool_get_available_size(tp), lcu_tpool_get_total_size(tp));

    lcu_tpool_destroy(&tp);
    lcu_sync_destroy(&worker_sync);

    lcu_logger_print("\n\n");
}