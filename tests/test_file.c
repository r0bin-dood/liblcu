#include <CUnit/CUnit.h>
#include "tests.h"
#include "lcu_file.h"

extern int force_malloc_failure;

void test_file_open_close()
{
    char *test_file = "./tests/out/test_file_open_close.txt";
    int ret = 0;

    FILE *fd = lcu_file_open(NULL, NULL);
    CU_ASSERT(fd == NULL);

    fd = lcu_file_open(test_file, NULL);
    CU_ASSERT(fd == NULL);

    fd = lcu_file_open(NULL, "w");
    CU_ASSERT(fd == NULL);

    // Open with a bad mode
    fd = lcu_file_open(test_file, "lll");
    CU_ASSERT(fd == NULL);

    fd = lcu_file_open(test_file, "wr");
    CU_ASSERT(fd != NULL);

    // Calling close with NULL
    ret = lcu_file_close(NULL);
    CU_ASSERT(ret == -1);

    ret = lcu_file_close(fd);
    CU_ASSERT(ret == 0);
}

void test_file_write_read()
{
    char *test_file = "./tests/out/test_file_write_read.txt";
    int ret = 0;

    FILE *fd = lcu_file_open(test_file, "wr");
    CU_ASSERT(fd != NULL);

    ret = lcu_file_write(NULL, NULL);
    CU_ASSERT(ret == -1);
    ret = lcu_file_write(fd, NULL);
    CU_ASSERT(ret == -1);
    lcu_buf_t some_str;
    ret = lcu_file_write(NULL, &some_str);
    CU_ASSERT(ret == -1);

    ret = lcu_file_read(NULL, NULL);
    CU_ASSERT(ret == -1);
    ret = lcu_file_read(fd, NULL);
    CU_ASSERT(ret == -1);
    ret = lcu_file_read(NULL, &some_str);
    CU_ASSERT(ret == -1);

    // Actually write
    char *test_str = "This is a test string";
    size_t test_str_sz = strlen(test_str);
    lcu_buf_init(&some_str, test_str, test_str_sz);

    ret = lcu_file_write(fd, &some_str);
    CU_ASSERT(ret == 0);

    ret = lcu_file_close(fd);
    CU_ASSERT(ret == 0);

    // Read
    some_str.buf = NULL;
    fd = lcu_file_open(test_file, "r");
    CU_ASSERT(fd != NULL);

    ret = lcu_file_read(fd, &some_str);
    CU_ASSERT(ret == 0);
    CU_ASSERT(strcmp(test_str, (char *)some_str.buf) == 0);

    ret = lcu_file_close(fd);
    CU_ASSERT(ret == 0);
}

void add_file_tests(CU_pSuite suite)
{
    CU_add_test(suite, "test_file_open_close", test_file_open_close);
    CU_add_test(suite, "test_file_write_read", test_file_write_read);
}