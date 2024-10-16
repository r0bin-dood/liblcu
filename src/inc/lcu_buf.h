#ifndef LCU_BUF_H
#define LCU_BUF_H

#include <stddef.h>
#include "lcu_sync.h"

typedef struct buf {
    void *buf;
    size_t size;
    bool alloc;
} lcu_buf_t;

void lcu_buf_init(lcu_buf_t *new_buf, void *data, size_t size);
void lcu_buf_create(lcu_buf_t *new_buf, const void *data, size_t size);
int lcu_buf_compare(const lcu_buf_t *b1, const lcu_buf_t *b2);
void lcu_buf_concat(lcu_buf_t *out, const lcu_buf_t *b1, const lcu_buf_t *b2);
void lcu_buf_destroy(lcu_buf_t *buf);

#endif // LCU_BUF_H