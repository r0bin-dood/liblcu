#ifndef LCU_ALLOC_H
#define LCU_ALLOC_H

#include <stddef.h>

void *lcu_alloc(size_t size);
void *lcu_zalloc(size_t size);
void lcu_free(void *buf);

#endif // LCU_ALLOC_H