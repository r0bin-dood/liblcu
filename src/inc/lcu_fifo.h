#ifndef LCU_FIFO_H
#define LCU_FIFO_H

#include <stddef.h>
#include "lcu.h"
#include "lcu_err.h"

typedef void * lcu_fifo_t;

lcu_fifo_t lcu_fifo_create(size_t max_hint);
size_t lcu_fifo_get_size(lcu_fifo_t handle);
lcu_err_t lcu_fifo_push(lcu_fifo_t handle, void *value, lcu_generic_callback cleanup_func);
void *lcu_fifo_peek(lcu_fifo_t handle);
lcu_err_t lcu_fifo_pop(lcu_fifo_t handle);
void lcu_fifo_destroy(lcu_fifo_t *handle);

#endif // LCU_FIFO_H