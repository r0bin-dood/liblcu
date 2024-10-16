#ifndef LCU_TPOOL_H
#define LCU_TPOOL_H

#include <stddef.h>
#include "lcu.h"

typedef void *lcu_tpool_t;

/**
 * @brief Creates a tpool object.
 * 
 * This function creates a pool of POSIX threads
 *
 * @param num_threads The number of threads to create
 * 
 * @return A tpool_t handle on success, NULL on error
 */
lcu_tpool_t lcu_tpool_create(size_t num_threads);
size_t lcu_tpool_get_total_size(lcu_tpool_t handle);
size_t lcu_tpool_get_available_size(lcu_tpool_t handle);
void lcu_tpool_grow(lcu_tpool_t handle, size_t num_threads);
int lcu_tpool_do_work(lcu_tpool_t handle, lcu_generic_callback func, void *args);
void lcu_tpool_destroy(lcu_tpool_t *handle);

#endif // LCU_TPOOL_H