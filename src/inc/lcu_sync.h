#ifndef LCU_SYNC_H
#define LCU_SYNC_H

#include <stdatomic.h>
#include <pthread.h>
#include "lcu.h"

#define ATOMIC_SPINLOCK(flag, block) do {                                       \
    while (atomic_flag_test_and_set_explicit(&(flag), memory_order_acquire)){}  \
    block;                                                                      \
    atomic_flag_clear_explicit(&(flag), memory_order_release);                  \
} while(0)

typedef struct sync {
    bool cond_var;
    pthread_cond_t cond;
    pthread_mutex_t mut;
} lcu_sync_t;

void lcu_sync_create(lcu_sync_t *sync);
void lcu_sync_lock(lcu_sync_t *sync);
void lcu_sync_unlock(lcu_sync_t *sync);
void lcu_sync_signal(lcu_sync_t *sync);
void lcu_sync_signal_n(lcu_sync_t *sync, size_t n);
void lcu_sync_wait(lcu_sync_t *sync);
void lcu_sync_destroy(lcu_sync_t *sync);

#endif // LCU_SYNC_H