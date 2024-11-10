#ifndef LCU_SLAB_ALLOC_H
#define LCU_SLAB_ALLOC_H

#include "lcu.h"

typedef void * lcu_slab_alloc_t;

lcu_slab_alloc_t lcu_slab_alloc_create(size_t chunk_size, size_t chunks_per_slab);
void *lcu_slab_alloc(lcu_slab_alloc_t handle);
void lcu_slab_free(lcu_slab_alloc_t handle, void *addr);
void lcu_slab_alloc_destroy(lcu_slab_alloc_t *handle);

#endif // LCU_SLAB_ALLOC_H