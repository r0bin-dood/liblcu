#include "lcu_sync.h"

static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void lcu_sync_create(lcu_sync_t *sync)
{
    sync->cond_var = false;
    pthread_cond_init(&sync->cond, NULL);
    pthread_mutex_init(&sync->mut, NULL);
}

void lcu_sync_lock(lcu_sync_t *sync)
{
    pthread_mutex_lock(&sync->mut);
}

void lcu_sync_unlock(lcu_sync_t *sync)
{
    pthread_mutex_unlock(&sync->mut);
}

void lcu_sync_signal(lcu_sync_t *sync)
{
    pthread_mutex_lock(&sync->mut);
    sync->cond_var = true;
    pthread_cond_signal(&sync->cond);
    pthread_mutex_unlock(&sync->mut);
}

void lcu_sync_signal_n(lcu_sync_t *sync, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        pthread_mutex_lock(&sync->mut);
        sync->cond_var = true;
        pthread_cond_signal(&sync->cond);
        pthread_mutex_unlock(&sync->mut);

        pthread_mutex_lock(&mut);
        pthread_cond_wait(&cond, &mut);
        pthread_mutex_unlock(&mut);
    }
}

void lcu_sync_wait(lcu_sync_t *sync)
{
    pthread_mutex_lock(&sync->mut);
    while (sync->cond_var == false)
        pthread_cond_wait(&sync->cond, &sync->mut);
    sync->cond_var = false;
    pthread_mutex_unlock(&sync->mut);

    pthread_mutex_lock(&mut);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mut);
}

void lcu_sync_destroy(lcu_sync_t *sync)
{
    pthread_cond_destroy(&sync->cond);
    pthread_mutex_destroy(&sync->mut);
}
