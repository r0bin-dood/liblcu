#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "lcu_sync.h"
#include "lcu_fifo.h"
#include "lcu_tpool.h"

typedef struct tpool {
    size_t num_threads;
    pthread_t *worker_id_arr;
    lcu_fifo_t worker_fifo;
    lcu_sync_t worker_fifo_mutex;
} tpool_t;

typedef struct worker {
    pthread_t worker_id;
    lcu_sync_t work_sync;
    lcu_generic_callback work_func;
    void *work_func_args;
} worker_t;

typedef struct worker_data {
    tpool_t *tpool;
    worker_t *worker;
} worker_data_t;

static void *lcu_helper_worker_func(void *arg);
static void lcu_helper_worker_cleanup(void *arg);

lcu_tpool_t lcu_tpool_create(size_t num_threads)
{
    if (num_threads == 0)
        return NULL;

    tpool_t *tpool = (tpool_t *) malloc(sizeof(tpool_t));
    if (tpool == NULL)
        return NULL;
    
    lcu_sync_create(&tpool->worker_fifo_mutex);
    
    tpool->worker_fifo = lcu_fifo_create(NULL);
    if (tpool->worker_fifo == NULL)
        goto error;

    tpool->num_threads = num_threads;
    tpool->worker_id_arr = (pthread_t *) malloc(sizeof(pthread_t) * num_threads);
    if (tpool->worker_id_arr == NULL)
        goto error;

    for (size_t i = 0; i < num_threads; i++)
    {
        worker_t *worker = (worker_t *) malloc(sizeof(worker_t));
        if (worker == NULL)
            goto error;
        worker->work_func = NULL;
        worker->work_func_args = NULL;

        lcu_sync_create(&worker->work_sync);
        
        worker_data_t *data = (worker_data_t *) malloc(sizeof(worker_data_t));
        if (data == NULL)
        {
            lcu_sync_destroy(&worker->work_sync);
            free(worker);
            goto error;
        }
        data->tpool = tpool;
        data->worker = worker;

        int ret = pthread_create(&worker->worker_id, NULL, &lcu_helper_worker_func, (void *)data);
        if (ret != 0)
        {
            lcu_sync_destroy(&worker->work_sync);
            free(worker);
            free(data);
            goto error;
        }
        tpool->worker_id_arr[i] = worker->worker_id;

        lcu_fifo_push(tpool->worker_fifo, (void *)worker);
    }

    return tpool;
    
    error:
    if (tpool->worker_id_arr != NULL)
        free(tpool->worker_id_arr);
    if (tpool->worker_fifo != NULL)
        lcu_fifo_destroy(&tpool->worker_fifo);
    lcu_sync_destroy(&tpool->worker_fifo_mutex);
    free(tpool);

    return NULL;
}

size_t lcu_tpool_get_total_size(lcu_tpool_t handle)
{
    tpool_t *tpool = (tpool_t *)handle;
    if (tpool == NULL)
        return 0;
    return tpool->num_threads;
}

size_t lcu_tpool_get_available_size(lcu_tpool_t handle)
{
    tpool_t *tpool = (tpool_t *)handle;
    if (tpool == NULL)
        return 0;
    return lcu_fifo_get_size(tpool->worker_fifo);
}

void lcu_tpool_grow(lcu_tpool_t handle, size_t num_threads)
{
    UNUSED(handle);
    UNUSED(num_threads);
}

int lcu_tpool_do_work(lcu_tpool_t handle, lcu_generic_callback func, void *args)
{
    tpool_t *tpool = (tpool_t *)handle;
    if (tpool == NULL)
        return -1;

    if (func == NULL)
        return -1;

    if (lcu_tpool_get_available_size(tpool) == 0)
        return -1;

    lcu_sync_lock(&tpool->worker_fifo_mutex);
    worker_t *worker = (worker_t *) lcu_fifo_peek(tpool->worker_fifo);
    worker->work_func = func;
    worker->work_func_args = args;
    lcu_fifo_pop(tpool->worker_fifo);
    lcu_sync_unlock(&tpool->worker_fifo_mutex);
    
    lcu_sync_signal(&worker->work_sync);

    return 0;
}

void lcu_tpool_destroy(lcu_tpool_t *handle)
{
    tpool_t **tpool = (tpool_t **)handle;
    if (tpool == NULL)
        return;
    if (*tpool == NULL)
        return;

    for (size_t i = 0; i < (*tpool)->num_threads; i++)
    {
        pthread_cancel((*tpool)->worker_id_arr[i]);
        pthread_join((*tpool)->worker_id_arr[i], NULL);
    }

    lcu_sync_lock(&(*tpool)->worker_fifo_mutex);
    lcu_fifo_destroy(&(*tpool)->worker_fifo);
    lcu_sync_unlock(&(*tpool)->worker_fifo_mutex);

    lcu_sync_destroy(&(*tpool)->worker_fifo_mutex);
    
    free((*tpool)->worker_id_arr);
    free((*tpool));
    *tpool = NULL;
}

void lcu_helper_worker_cleanup(void *arg)
{
    worker_data_t *data = (worker_data_t *)arg;
    worker_t *worker = data->worker;

    lcu_sync_destroy(&worker->work_sync);

    free(worker);
    free(data);
}

void *lcu_helper_worker_func(void *arg)
{
    worker_data_t *data = (worker_data_t *)arg;
    worker_t *worker = data->worker;
    tpool_t *tpool = data->tpool;

    pthread_cleanup_push(lcu_helper_worker_cleanup, arg);

    while (true)
    {
        lcu_sync_wait(&worker->work_sync);

        (*worker->work_func)(worker->work_func_args);
        worker->work_func = NULL;
        worker->work_func_args = NULL;

        lcu_sync_lock(&tpool->worker_fifo_mutex);
        lcu_fifo_push(tpool->worker_fifo, (void *)worker);
        lcu_sync_unlock(&tpool->worker_fifo_mutex);
    }

    pthread_cleanup_pop(true);

    return NULL;
}
