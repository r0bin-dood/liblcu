
#include <unistd.h>
#include "main.h"

struct worker {
    int id;
    void *data;
};

void test_func(void *arg)
{
    struct worker *w_p = (struct worker *) arg;
    lcu_logger_print("   from %d: %s\n", w_p->id, (char *)w_p->data);
}

void example_tpool()
{
    lcu_logger_print("%s():\n", __func__);

    lcu_tpool_t tp = lcu_tpool_create(4);

    lcu_logger_print("avail workers: %lu, total: %lu\n", lcu_tpool_get_available_size(tp), lcu_tpool_get_total_size(tp));

    struct worker w1 = {
        .id = 1,
        .data = (char *)"hello"
    };
    struct worker w2 = {
        .id = 2,
        .data = (char *)"world"
    };
    lcu_logger_print("add worker: %d %s\n", w1.id, (char *)w1.data);
    lcu_logger_print("add worker: %d %s\n", w2.id, (char *)w2.data);
    lcu_tpool_do_work(tp, &test_func, &w1);
    lcu_tpool_do_work(tp, &test_func, &w2);
    lcu_logger_print("before workers finish - avail workers: %lu, total: %lu\n", lcu_tpool_get_available_size(tp), lcu_tpool_get_total_size(tp));
    usleep(100 * 1000);
    lcu_logger_print("after workers finished - avail workers: %lu, total: %lu\n", lcu_tpool_get_available_size(tp), lcu_tpool_get_total_size(tp));

    lcu_tpool_destroy(&tp);

    lcu_logger_print("\n\n");
}