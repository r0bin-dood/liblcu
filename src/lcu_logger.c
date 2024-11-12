#include <pthread.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "lcu.h"
#include "lcu_fifo.h"
#include "lcu_logger.h"

#define LCU_BUF_SIZE_DEFAULT 128
#define LCU_BUF_SIZE_INCREASE 32

static bool logger_running = true;

static pthread_t tid;
static FILE *fd;
static lcu_fifo_t str_fifo;
static pthread_mutex_t str_fifo_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t print_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

static void *lcu_helper_logger_func(void *arg);
static void lcu_helper_str_cleanup(void *buf);

int lcu_logger_create(const char *out)
{
    if (out == NULL)
        return -1;

    str_fifo = lcu_fifo_create(1024);
    if (str_fifo == NULL)
        return -1;

    if (strcmp(out, LCU_STDOUT) == 0)
        fd = stdout;
    else if (strcmp(out, LCU_STDERR) == 0)
        fd = stderr;
    else
        fd = fopen(out, "w");
    if (fd == NULL)
    {
        lcu_fifo_destroy(&str_fifo);
        return -1;
    }

    logger_running = true;
    
    int ret = pthread_create(&tid, NULL, &lcu_helper_logger_func, NULL);
    if (ret != 0)
    {
        fclose(fd);
        lcu_fifo_destroy(&str_fifo);
        return -1;
    }

    return 0;
}

void lcu_logger_print(const char *fmt, ...)
{
    if (fmt == NULL)
        return;

    int n;
    int size = LCU_BUF_SIZE_DEFAULT;
    char *buf;
    char *new_buf;
    va_list ap;

    buf = (char *) malloc(size);
    if (buf == NULL)
        return;

    while (true)
    {
        va_start(ap, fmt);
        n = vsnprintf(buf, size, fmt, ap);
        va_end(ap);

        if (n < 0)
        {
            free(buf);
            return;
        }

        if (n < size)
            break;

        size = n + LCU_BUF_SIZE_INCREASE;

        new_buf = (char *) realloc(buf, size);
        if (new_buf == NULL)
        {
            free(buf);
            return;
        }
        else 
        {
            buf = new_buf;
        }
    }
    
    pthread_mutex_lock(&str_fifo_mutex);
    if (str_fifo != NULL)
        lcu_fifo_push(str_fifo, (void *)buf, &lcu_helper_str_cleanup);
    pthread_mutex_unlock(&str_fifo_mutex);

    pthread_mutex_lock(&print_mutex);
    pthread_cond_signal(&print_cond);
    pthread_mutex_unlock(&print_mutex);
}

void lcu_logger_destroy()
{
    if (str_fifo == NULL)
        return;

    logger_running = false;

    pthread_mutex_lock(&print_mutex);
    pthread_cond_signal(&print_cond);
    pthread_mutex_unlock(&print_mutex);

    pthread_join(tid, NULL);

    pthread_mutex_lock(&str_fifo_mutex);
    lcu_fifo_destroy(&str_fifo);
    pthread_mutex_unlock(&str_fifo_mutex);

    if (fd != NULL)
    {
        fclose(fd);
        fd = NULL;
    }
}

void *lcu_helper_logger_func(void *arg)
{
    UNUSED(arg);
    
    struct timespec tm;
    tm.tv_sec = 0;
    tm.tv_nsec = 50000000; // 50 ms

    while (logger_running || lcu_fifo_get_size(str_fifo) > 0)
    {
        pthread_mutex_lock(&print_mutex);
        int ret = pthread_cond_timedwait(&print_cond, &print_mutex, &tm);
        pthread_mutex_unlock(&print_mutex);
        if (ret == ETIMEDOUT)
        {
            if (lcu_fifo_get_size(str_fifo) == 0)
                continue;
        }

        char *str = (char *) lcu_fifo_peek(str_fifo);
        if (str != NULL)
        {
            size_t size = strlen(str);
            fwrite(str, sizeof(char), size, fd);
            fflush(fd);
            pthread_mutex_lock(&str_fifo_mutex);
            lcu_fifo_pop(str_fifo);
            pthread_mutex_unlock(&str_fifo_mutex);
        }
    }

    return NULL;
}

void lcu_helper_str_cleanup(void *buf)
{
    if (buf != NULL)
        free(buf);
}
