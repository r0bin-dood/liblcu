#include <stdint.h>
#include "lcu_sync.h"
#include "lcu_alloc.h"
#include "lcu_slab_alloc.h"

#define ALLOC_MEMORY_MARK 0x11

#define ADJUST_CHUNK_ADDRESS(chunk, add_offset)  \
    (void *)((uint8_t *)(chunk) + ((add_offset) ? (sizeof(void *) + 1) : -(sizeof(void *) + 1)))

#define DEREF_CHUNK_MARK(chunk)  (*((uint8_t *)(chunk) + sizeof(void *)))

typedef struct slab {
    uint8_t *chunk;
    struct slab *next;
} slab_t;

typedef struct slab_alloc {
    slab_t *slab;
    slab_t *slab_tail;
    void *free_list;
    void *free_list_tail;
    size_t chunks_per_slab;
    size_t chunk_size;
    size_t chunk_size_total;
} slab_alloc_t;

static slab_t *insert_slab(slab_alloc_t *sa);
static void insert_free_list(slab_alloc_t *sa, slab_t *slab);
static void push_free_list(slab_alloc_t *sa, void *chunk);
static void *pop_free_list(slab_alloc_t *sa);
static bool is_in_free_list(slab_alloc_t *sa, void *addr);
static bool is_valid_chunk_address(slab_alloc_t *sa, void *addr);

lcu_slab_alloc_t lcu_slab_alloc_create(size_t chunk_size, size_t chunks_per_slab)
{
    slab_alloc_t *sa = lcu_zalloc(sizeof(slab_alloc_t));
    if (sa != NULL) {
        sa->chunks_per_slab = chunks_per_slab;
        sa->chunk_size = chunk_size;
        sa->chunk_size_total = chunk_size * chunks_per_slab;
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

    if (sa->free_list == NULL) {
        if (insert_slab(sa) == NULL)
            return NULL;
    }
    
    return pop_free_list(sa);
}

void lcu_slab_free(lcu_slab_alloc_t handle, void *addr)
{
    if (handle == NULL || addr == NULL)
        return;

    slab_alloc_t *sa = (slab_alloc_t *)handle;

    if (is_in_free_list(sa, addr) == true)
        return;

    if (is_valid_chunk_address(sa, addr) == true)
        push_free_list(sa, addr);
}

void lcu_slab_alloc_destroy(lcu_slab_alloc_t *handle)
{
    if (handle == NULL || *handle == NULL)
        return;

    slab_alloc_t *sa = (slab_alloc_t *)(*handle);

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

        size_t total_chunk_size_with_free_list = sa->chunk_size_total + sa->chunks_per_slab * (sizeof(void *) + 1);
        slab->chunk = lcu_zalloc(total_chunk_size_with_free_list);
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
    void *chunk;
    for (size_t i = 0; i < sa->chunks_per_slab; i++) {
        chunk = (void *)(slab->chunk + (i * sa->chunk_size));
        push_free_list(sa, ADJUST_CHUNK_ADDRESS(chunk, true));
    }
}

void push_free_list(slab_alloc_t *sa, void *chunk)
{
    chunk = ADJUST_CHUNK_ADDRESS(chunk, false);
    DEREF_CHUNK_MARK(chunk) = 0;
    *(void **)chunk = NULL;
    if (sa->free_list == NULL) {
        sa->free_list = chunk;
        sa->free_list_tail = chunk;
    } else {
        *(void **)sa->free_list_tail = chunk;
        sa->free_list_tail = chunk;
    }
}

void *pop_free_list(slab_alloc_t *sa)
{
    if (sa->free_list == NULL)
        return NULL;
    void *chunk = sa->free_list;
    sa->free_list = *(void **)chunk;
    DEREF_CHUNK_MARK(chunk) = ALLOC_MEMORY_MARK;
    return ADJUST_CHUNK_ADDRESS(chunk, true);
}

bool is_in_free_list(slab_alloc_t *sa, void *addr)
{
    void *chunk = ADJUST_CHUNK_ADDRESS(addr, false);
    if (DEREF_CHUNK_MARK(chunk) == ALLOC_MEMORY_MARK)
        return false;
    return true;
}

bool is_valid_chunk_address(slab_alloc_t *sa, void *addr)
{
    slab_t *slab = sa->slab;
    while (slab != NULL) {
        uint8_t *slab_start = slab->chunk;
        uint8_t *slab_end = slab_start + sa->chunk_size_total;
        if (addr >= (void *)slab_start && addr < (void *)slab_end) {
            size_t offset = (uintptr_t)addr - (uintptr_t)slab_start;
            if (offset % sa->chunk_size == 0)
                return true;
        }
        slab = slab->next;
    }
    return false;
}