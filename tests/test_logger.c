#include <CUnit/CUnit.h>
#include "tests.h"
#include "lcu_logger.h"
#include "lcu_sync.h"
#include "lcu_tpool.h"
#include "lcu_file.h"

#define NUM_THREADS 10

typedef struct w_data {
    lcu_sync_t w_sync;
    char *w_str;
} w_data_t;

static void test_worker(void *arg)
{
    w_data_t *w_data = (w_data_t *)arg;

    lcu_sync_wait(&w_data->w_sync);

    lcu_logger_print(w_data->w_str);
}

void test_logger_create_destroy(void)
{
    // Call destroy before calling create
    lcu_logger_destroy();

    int ret = 0;
    ret = lcu_logger_create(NULL);
    CU_ASSERT(ret == -1);

    char *test_file = "./tests/out/test_logger_create_destroy.txt";
    ret = lcu_logger_create(test_file);
    CU_ASSERT(ret == 0);

    // Testing print
    lcu_logger_print("From logger test\n");

    lcu_logger_destroy();
}

char *long_str = "\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
0000000000000000000000000000000000000000000000000-\
";
void test_logger_print(void)
{
    // Call print before create
    lcu_logger_print("Hello, World!\n");

    char *test_file = "./tests/out/test_logger_print.txt";
    int ret = lcu_logger_create(test_file);
    CU_ASSERT(ret == 0);

    // Print NULL
    lcu_logger_print(NULL);

    // Testing print
    lcu_logger_print("From logger test\n");

    // Print something long
    lcu_logger_print(long_str);
    lcu_logger_print("\n");

    lcu_logger_print("After malloc failure\n");

    lcu_logger_destroy();

    // Call print after destroy
    lcu_logger_print("Hello, World!\n");
}

void test_logger_10_thread_print(void)
{
    char *test_file = "./tests/out/test_logger_10_thread_print.txt";
    int ret = lcu_logger_create(test_file);
    CU_ASSERT(ret == 0);

    lcu_tpool_t tp = NULL;
    tp = lcu_tpool_create(NUM_THREADS);
    CU_ASSERT(tp != NULL);
    CU_ASSERT(lcu_tpool_get_available_size(tp) == NUM_THREADS);

    w_data_t w_data[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++)
    {
        w_data[i].w_str = long_str;
        lcu_sync_create(&w_data[i].w_sync);

        ret = lcu_tpool_do_work(tp, &test_worker, &w_data[i]);
        CU_ASSERT(ret == 0);
    }
    usleep(10000);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        lcu_sync_signal(&w_data[i].w_sync);
        lcu_sync_destroy(&w_data[i].w_sync);
    }

    lcu_tpool_destroy(&tp);
    CU_ASSERT(tp == NULL);

    lcu_logger_destroy();
    
    // Evaluate prints
    FILE *fd = lcu_file_open(test_file, "r");
    CU_ASSERT(fd != NULL);

    size_t file_size = lcu_file_get_size(fd);
    size_t expected_size = strlen(long_str) * NUM_THREADS;
    CU_ASSERT(file_size == expected_size);

    lcu_buf_t file_content;
    ret = lcu_file_read(fd, &file_content);
    CU_ASSERT(ret == 0);

    char *buf = (char *)file_content.buf;
    for (int i = 1; i < NUM_THREADS; i++)
        CU_ASSERT(buf[(i * (strlen(long_str))) - 1] == '-');

    lcu_file_close(fd);
}

void add_logger_tests(CU_pSuite suite)
{
    CU_add_test(suite, "test_logger_create_destroy", test_logger_create_destroy);
    CU_add_test(suite, "test_logger_print", test_logger_print);
    CU_add_test(suite, "test_logger_10_thread_print", test_logger_10_thread_print);
}