#include <stdlib.h>
#include "lcu_list.h"
#include "lcu_sync.h"
#include "lcu_alloc.h"
#include "lcu_slab_alloc.h"

#define LIST(x) ((list_t *)x)

#define ENUM_CACHE_LEVELS \
    X(0) \
    X(1) \
    X(2) \
    X(3) \
    X(4) \
    X(5) \
    X(6) \
    X(7)
#define X(n) CACHE_LEVEL_##n = (1 << (2 * (n + 3))),
enum {
    ENUM_CACHE_LEVELS
    CACHE_LEVELS_MAX = 8,
};
static const size_t cache_intervals[CACHE_LEVELS_MAX] = {
    CACHE_LEVEL_0, CACHE_LEVEL_1, CACHE_LEVEL_2, CACHE_LEVEL_3,
    CACHE_LEVEL_4, CACHE_LEVEL_5, CACHE_LEVEL_6, CACHE_LEVEL_7
};

/**
 * \internal
 * This struct is used internally.
 */
typedef struct list_node {
    void *value;
    lcu_generic_callback cleanup_func;
    struct list_node *prev;
    struct list_node *next;
    struct list_node *skip_next[CACHE_LEVELS_MAX]; 
} list_node_t;

typedef struct list_fat_node {
    int index;
    list_node_t *node;
} list_fat_node_t;

/**
 * \internal
 * This struct is used internally.
 */
typedef struct list {
    size_t size;
    atomic_flag flag;
    list_fat_node_t cursor;
    list_node_t *head;
    list_node_t *tail;
    bool invalid_skip_list;
    list_node_t *skip_list[CACHE_LEVELS_MAX];
    list_node_t *skip_tail[CACHE_LEVELS_MAX];
    lcu_slab_alloc_t allocator;
} list_t;

static list_node_t *new_list_node(list_t *list, void *value, lcu_generic_callback cleanup_func);
static list_node_t *get_list_node(list_t *list, int i);
static inline void update_cursor(list_t *list, int index, list_node_t *node);
static inline void update_node_ptrs(list_node_t *node, list_node_t *prev, list_node_t *next);
static void add_skip_list(list_t *list, list_node_t *node);

lcu_list_t lcu_list_create(size_t max_hint)
{
    list_t *list = lcu_zalloc(sizeof(list_t));
    if (list != NULL) {
        atomic_flag_clear(&list->flag);
        list->allocator = lcu_slab_alloc_create(sizeof(list_node_t), max_hint);
        if (list->allocator == NULL) {
            lcu_free(list);
            list = NULL;
        }
    }
    return list;
}

size_t lcu_list_get_size(lcu_list_t handle)
{
    if (handle == NULL)
        return 0;
    return LIST(handle)->size;
}

int lcu_list_insert_front(lcu_list_t handle, void *value, lcu_generic_callback cleanup_func)
{
    if (handle == NULL)
        return -1;

    list_t *list = LIST(handle);

    list_node_t *new_node = new_list_node(list, value, cleanup_func);
    if (new_node == NULL)
        return -1;

    ATOMIC_SPINLOCK(list->flag, {
        if (list->size == 0) {
            update_node_ptrs(new_node, NULL, NULL);
            list->tail = new_node;
            update_cursor(list, 0, new_node);
        } else {
            update_node_ptrs(new_node, NULL, list->head);
            list->head->prev = new_node;
            list->cursor.index++;
        }
        list->head = new_node;
        list->size++;

        list->invalid_skip_list = true;
    });

    return 0;
}

int lcu_list_insert_back(lcu_list_t handle, void *value, lcu_generic_callback cleanup_func)
{
    if (handle == NULL)
        return -1;

    list_t *list = LIST(handle);

    list_node_t *new_node = new_list_node(list, value, cleanup_func);
    if (new_node == NULL)
        return -1;

    ATOMIC_SPINLOCK(list->flag, {
        if (list->size == 0) {
            update_node_ptrs(new_node, NULL, NULL);
            list->head = new_node;
            update_cursor(list, 0, new_node);
        } else {
            update_node_ptrs(new_node, list->tail, NULL);
            list->tail->next = new_node;
        }
        list->tail = new_node;
        list->size++;
        add_skip_list(list, new_node);
    });

    return 0;
}

int lcu_list_insert(lcu_list_t handle, int i, void *value, lcu_generic_callback cleanup_func)
{
    if (handle == NULL)
        return -1;

    if (i < 0 || i > LIST(handle)->size)
        return -1;

    list_t *list = LIST(handle);

    if (i == 0)
        return lcu_list_insert_front(list, value, cleanup_func);
    if (i == list->size)
        return lcu_list_insert_back(list, value, cleanup_func);

    list_node_t *old_node = NULL;
    ATOMIC_SPINLOCK(list->flag, {
        old_node = get_list_node(list, i);
    });

    list_node_t *new_node = new_list_node(list, value, cleanup_func);
    if (new_node == NULL)
        return -1;

    ATOMIC_SPINLOCK(list->flag, {
        update_node_ptrs(new_node, old_node->prev, old_node);
        old_node->prev->next = new_node;
        old_node->prev = new_node;
        list->cursor.index++;

        list->size++;

        list->invalid_skip_list = true;
    });

    return 0;
}

void *lcu_list_peek_front(lcu_list_t handle)
{
    if (handle != NULL && LIST(handle)->head != NULL)
        return LIST(handle)->head->value;
    return NULL;
}

void *lcu_list_peek_back(lcu_list_t handle)
{
    if (handle != NULL && LIST(handle)->tail != NULL)
        return LIST(handle)->tail->value;
    return NULL;
}

void *lcu_list_peek(lcu_list_t handle, int i)
{
    list_t *list = LIST(handle);

    if (list == NULL || list->size == 0)
        return NULL;
    
    const size_t list_size = list->size;

    if (i < 0 || i >= list_size)
        return NULL;

    if (i == 0)
        return lcu_list_peek_front(list);
    if (i == list_size - 1)
        return lcu_list_peek_back(list);

    list_node_t *node;
    ATOMIC_SPINLOCK(list->flag, {
        node = get_list_node(list, i);
    });

    return node->value;
}

int lcu_list_move_to_front(lcu_list_t handle, int i)
{
    if (LIST(handle) == NULL ||
        LIST(handle)->size == 0)
        return -1;

    if (i < 0 || i >= LIST(handle)->size)
        return -1;

    if (i == 0)
        return 0;

    list_t *list = LIST(handle);

    list_node_t *node;
    ATOMIC_SPINLOCK(list->flag, {
        if (i == list->size - 1) {
            node = list->tail;
            list->tail->prev->next = NULL;
            list->tail = list->tail->prev;
        } else {
            node = get_list_node(list, i);
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
        update_cursor(list, i, node->prev);
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;

        list->invalid_skip_list = true;
    });

    return 0;
}

int lcu_list_move_to_back(lcu_list_t handle, int i)
{
    if (LIST(handle) == NULL ||
        LIST(handle)->size == 0)
        return -1;

    if (i < 0 || i >= LIST(handle)->size)
        return -1;

    if (i == LIST(handle)->size - 1)
        return 0;
    
    list_t *list = LIST(handle);

    list_node_t *node;
    ATOMIC_SPINLOCK(list->flag, {
        if (i == 0) {
            node = list->head;
            list->head->next->prev = NULL;
            list->head = list->head->next;
        } else {
            node = get_list_node(list, i);
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
        update_cursor(list, i, node->next);
        node->next = NULL;
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;

        list->invalid_skip_list = true;
    });

    return 0;
}

int lcu_list_move(lcu_list_t handle, int from, int to)
{
    list_t *list = LIST(handle);

    if (list == NULL ||
        list->size == 0)
        return -1;

    if ((from < 0 || from >= list->size) ||
        (to < 0 || to >= list->size))
        return -1;

    if (from == to)
        return 0;

    if (to == 0)
        return lcu_list_move_to_front(handle, from);
    if (to == list->size - 1)
        return lcu_list_move_to_back(handle, from);

    list_node_t *node;
    list_node_t *dst_node;

    ATOMIC_SPINLOCK(list->flag, {
        if (from == 0) {
            node = list->head;
            list->head = list->head->next;
            list->head->prev = NULL;
        } else if (from == list->size - 1) {
            node = list->tail;
            list->tail = list->tail->prev;
            list->tail->next = NULL;
        } else {
            node = get_list_node(list, from);
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
        if (abs(from - to) == 1) {
            if (from < to) {
                dst_node = node->next;
                node->prev = dst_node;
                node->next = dst_node->next;
                dst_node->next->prev = node;
                dst_node->next = node;
            } else {
                dst_node = node->prev;
                node->next = dst_node;
                node->prev = dst_node->prev;
                dst_node->prev->next = node;
                dst_node->prev = node;
            }
        } else {
            dst_node = get_list_node(list, to);
            node->next = dst_node;
            node->prev = dst_node->prev;
            dst_node->prev->next = node;
            dst_node->prev = node;
        }
        update_cursor(list, to, node);

        list->invalid_skip_list = true;
    });
    
    return 0;
}

int lcu_list_swap_with_front(lcu_list_t handle, int i)
{
    if (lcu_list_move(handle, i, 0) == 0)
        return lcu_list_move(handle, 1, i + 1);
    return -1;
}

int lcu_list_swap_with_back(lcu_list_t handle, int i)
{
    if (lcu_list_move(handle, i, LIST(handle)->size - 1) == 0)
        return lcu_list_move(handle, (LIST(handle)->size - 2), i + 1);
    return -1;
}

int lcu_list_swap(lcu_list_t handle, int i, int j)
{
    if (lcu_list_move(handle, i, j) == 0)
        return lcu_list_move(handle, j + 1, i + 1);
    return -1;
}

int lcu_list_remove_front(lcu_list_t handle)
{
    if (handle == NULL || 
        LIST(handle)->size == 0)
        return -1;

    list_t *list = LIST(handle);

    list_node_t *node;
    ATOMIC_SPINLOCK(list->flag, {
        node = list->head;

        if (list->cursor.node == node)
            update_cursor(list, 0, node->next);
        else if (list->cursor.index > 0)
            list->cursor.index--;

        if (list->size == 1) {
            list->head = NULL;
            list->tail = NULL;
        } else {
            list->head = node->next;
            list->head->prev = NULL;
        }
        list->size--;

        list->invalid_skip_list = true;
    });

    if (node->cleanup_func != NULL)
        (*node->cleanup_func)(node->value);
    lcu_slab_free(list->allocator, node);

    return 0;
}

int lcu_list_remove_back(lcu_list_t handle)
{    
    if (handle == NULL || 
        LIST(handle)->size == 0)
        return -1;

    list_t *list = LIST(handle);

    list_node_t *node;
    ATOMIC_SPINLOCK(list->flag, {
        node = list->tail;

        if (list->cursor.node == node)
            update_cursor(list, list->cursor.index - 1, node->prev);

        if (list->size == 1) {
            list->head = NULL;
            list->tail = NULL;
        } else {
            list->tail = node->prev;
            list->tail->next = NULL;
        }
        list->size--;
    });

    if (node->cleanup_func != NULL)
        (*node->cleanup_func)(node->value);
    lcu_slab_free(list->allocator, node);

    return 0;
}

int lcu_list_remove(lcu_list_t handle, int i)
{
    if (LIST(handle) == NULL || 
        LIST(handle)->size == 0)
        return -1;

    if (i < 0 || i > LIST(handle)->size)
        return -1;

    if (i == 0)
        return lcu_list_remove_front(handle);
    if (i == LIST(handle)->size)
        return lcu_list_remove_back(handle);

    list_t *list = LIST(handle);

    list_node_t *node;
    ATOMIC_SPINLOCK(list->flag, {
        node = get_list_node(list, i);
        update_cursor(list, i + 1, node->next);
        node->prev->next = node->next;
        node->next->prev = node->prev;
        list->size--;

        list->invalid_skip_list = true;
    });
    
    if (node->cleanup_func != NULL)
        (*node->cleanup_func)(node->value);
    lcu_slab_free(list->allocator, node);

    return 0;
}

void lcu_list_destroy(lcu_list_t *handle)
{
    if (handle == NULL || 
        *handle == NULL)
        return;

    list_t *list = LIST(*handle);
    size_t list_size = list->size;
    for (size_t i = 0; i < list_size; i++)
        lcu_list_remove_back(list);
    
    lcu_slab_alloc_destroy(&list->allocator);

    lcu_free(list);
    *handle = NULL;
}

list_node_t *new_list_node(list_t *list, void *value, lcu_generic_callback cleanup_func)
{
    list_node_t *new_node = lcu_slab_alloc(list->allocator);
    if (new_node != NULL) {
        new_node->value = value;
        new_node->cleanup_func = cleanup_func;
    }
    return new_node;
}

list_node_t *get_list_node(list_t *list, int i)
{
    list_node_t *node;
    int current_index;
    const int list_size = list->size;
    const int distance_to_cursor = abs(list->cursor.index - i);
    int steps_per_level[CACHE_LEVELS_MAX];

    if (i < (list_size >> 1)) {
        node = list->head;
        current_index = 0;
    } else if (distance_to_cursor <= (list_size >> 2)) {
        node = list->cursor.node;
        current_index = list->cursor.index;
    } else {
        node = list->tail;
        current_index = list_size - 1;
    }

    if (list->invalid_skip_list != true) {
        if (abs(current_index - i) >= CACHE_LEVEL_0) {
            current_index = 0;
            list_node_t *skip_node = NULL;
            int target_index = i;
            for (int level = CACHE_LEVELS_MAX - 1; level >= 0; level--) {
                steps_per_level[level] = target_index / cache_intervals[level];
                target_index %= cache_intervals[level];
                current_index += steps_per_level[level] * cache_intervals[level];
            }
            for (int level = CACHE_LEVELS_MAX - 1; level >= 0; level--) {
                int steps = steps_per_level[level];
                if (steps == 0)
                    continue;
                if (skip_node == NULL) {
                    skip_node = list->skip_list[level];
                    steps--;
                }
                while (steps--) {
                    skip_node = skip_node->skip_next[level];
                }
            }
            node = skip_node;
            if (current_index <= i)
                node = node->next;
            else
                node = node->prev;
        }
    }

    if (current_index < i) {
        while (current_index < i) {
            node = node->next;
            current_index++;
        }
    } else if (current_index > i) {
        while (current_index > i) {
            node = node->prev;
            current_index--;
        }
    }
    update_cursor(list, i, node);
    return node;
}

void update_cursor(list_t *list, int index, list_node_t *node)
{
    list->cursor.index = index;
    list->cursor.node = node;
}

void update_node_ptrs(list_node_t *node, list_node_t *prev, list_node_t *next)
{
    node->prev = prev;
    node->next = next;
}

void add_skip_list(list_t *list, list_node_t *node)
{
    const size_t list_size = list->size;
    for (size_t level = 0; level < CACHE_LEVELS_MAX; level++) {
        if ((list_size & (cache_intervals[level] - 1)) == 0) {
            if (list->skip_list[level] == NULL) {
                list->skip_list[level] = node;
            } else {
                list->skip_tail[level]->skip_next[level] = node;
            }
            list->skip_tail[level] = node;
            node->skip_next[level] = NULL;
        }
    }
}
