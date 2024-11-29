#include <stdint.h>
#include "lcu_sync.h"
#include "lcu_alloc.h"
#include "lcu_slab_alloc.h"
#include <stdio.h>

typedef struct chunk {
    struct chunk *next;
    uint32_t mark;
    uint8_t data[];
} chunk_t;

#define ALLOC_MEMORY_MARK   (uint32_t)0xFEEEFEEE
#define TO_CHUNK(addr) (chunk_t *)((uint8_t *)(addr) - sizeof(chunk_t))

typedef struct slab {
    struct slab *next;
    chunk_t *chunk;
} slab_t;

typedef struct slab_alloc {
    atomic_flag flag;
    slab_t *slab;
    slab_t *slab_tail;
    chunk_t *free_list;
    chunk_t *free_list_tail;
    size_t chunks_per_slab;
    size_t chunk_size;
    size_t chunk_size_total;
    size_t total_chunks_in_slab;
} slab_alloc_t;

static slab_t *insert_slab(slab_alloc_t *sa);
static void insert_free_list(slab_alloc_t *sa, slab_t *slab);
static void push_free_list(slab_alloc_t *sa, chunk_t *chunk);
static void *pop_free_list(slab_alloc_t *sa);
static bool is_in_free_list(slab_alloc_t *sa, chunk_t *addr);

lcu_slab_alloc_t lcu_slab_alloc_create(size_t chunk_size, size_t chunks_per_slab)
{
    slab_alloc_t *sa = lcu_zalloc(sizeof(slab_alloc_t));
    if (sa != NULL) {
        atomic_flag_clear(&sa->flag);
        sa->chunk_size = chunk_size;
        sa->chunks_per_slab = chunks_per_slab;
        sa->chunk_size_total = chunk_size + sizeof(chunk_t);
        sa->total_chunks_in_slab = sa->chunk_size_total * chunks_per_slab;
        sa->slab = insert_slab(sa);
        if (sa->slab == NULL) {
            lcu_free(sa);
            sa = NULL;
        }
    }
    return sa;
}

void *lcu_slab_alloc(lcu_slab_alloc_t handle)
{
    if (handle == NULL)
        return NULL;
    
    slab_alloc_t *sa = (slab_alloc_t *)handle;

    bool ret_null = false;
    ATOMIC_SPINLOCK(sa->flag, {
        if (sa->free_list == NULL) {
            if (insert_slab(sa) == NULL)
                ret_null = true;
        }
    });
    if (ret_null == true)
        return NULL;
    
    return pop_free_list(sa);
}

void lcu_slab_free(lcu_slab_alloc_t handle, void *addr)
{
    if (handle == NULL || addr == NULL)
        return;

    slab_alloc_t *sa = (slab_alloc_t *)handle;

    chunk_t *chunk = TO_CHUNK(addr);
    if (is_in_free_list(sa, chunk) == false) {
        ATOMIC_SPINLOCK(sa->flag, {
            push_free_list(sa, chunk);
        });
    }
}

void lcu_slab_alloc_destroy(lcu_slab_alloc_t *handle)
{
    if (handle == NULL || *handle == NULL)
        return;

    slab_alloc_t *sa = (slab_alloc_t *)(*handle);

    ATOMIC_SPINLOCK(sa->flag, {
        slab_t *slab = sa->slab;
        slab_t *next_slab;
        while (slab != NULL) {
            next_slab = slab->next;
            lcu_free(slab->chunk);
            lcu_free(slab);
            slab = next_slab;
        }
        sa->slab = NULL;
        sa->slab_tail = NULL;
    });
    lcu_free(sa);
    *handle = NULL;
}

slab_t *insert_slab(slab_alloc_t *sa)
{
    slab_t *slab = lcu_alloc(sizeof(slab_t));
    if (slab != NULL) {
        if (sa->slab_tail != NULL)
            sa->slab_tail->next = slab;
        sa->slab_tail = slab;
        slab->next = NULL;
        slab->chunk = lcu_zalloc(sa->total_chunks_in_slab);
        if (slab->chunk == NULL) {
            lcu_free(slab);
            return NULL;
        }
        insert_free_list(sa, slab);
    }
    return slab;
}

void insert_free_list(slab_alloc_t *sa, slab_t *slab)
{
    chunk_t *chunk;
    for (size_t i = 0; i < sa->chunks_per_slab; i++) {
        chunk = (chunk_t *)((uint8_t *)slab->chunk + (i * sa->chunk_size_total));
        push_free_list(sa, chunk);
    }
}

void push_free_list(slab_alloc_t *sa, chunk_t *chunk)
{
    chunk->mark = 0;
    chunk->next = NULL;
    if (sa->free_list == NULL)
        sa->free_list = chunk;
    else
        sa->free_list_tail->next = chunk;
    sa->free_list_tail = chunk;
}

void *pop_free_list(slab_alloc_t *sa)
{
    chunk_t *chunk = NULL;
    ATOMIC_SPINLOCK(sa->flag, {
        if (sa->free_list != NULL) {
            chunk = sa->free_list;
            sa->free_list = chunk->next;
        }
    });
    if (chunk == NULL)
        return NULL;
    chunk->mark = ALLOC_MEMORY_MARK;
    return chunk->data;
}

bool is_in_free_list(slab_alloc_t *sa, chunk_t *chunk)
{
    if (chunk->mark == ALLOC_MEMORY_MARK)
        return false;
    return true;
}
