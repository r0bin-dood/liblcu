#include "lcu_sync.h"

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

void lcu_sync_wait(lcu_sync_t *sync)
{
    pthread_mutex_lock(&sync->mut);
    while (sync->cond_var == false)
        pthread_cond_wait(&sync->cond, &sync->mut);
    sync->cond_var = false;
    pthread_mutex_unlock(&sync->mut);
}

void lcu_sync_destroy(lcu_sync_t *sync)
{
    pthread_cond_destroy(&sync->cond);
    pthread_mutex_destroy(&sync->mut);
}
