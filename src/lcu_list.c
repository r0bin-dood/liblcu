#include <stdlib.h>
#include "lcu_list.h"

#define LIST(x) ((list_t *)x)

/**
 * \internal
 * This struct is used internally.
 */
typedef struct list_node {
    void *value;
    lcu_generic_callback cleanup_func;
    struct list_node *prev;
    struct list_node *next;
} list_node_t;

/**
 * \internal
 * This struct is used internally.
 */
typedef struct list_cache {
    int index;
    list_node_t *node;
} list_cache_t;

/**
 * \internal
 * This struct is used internally.
 */
typedef struct list {
    size_t size;
    list_cache_t cursor;
    list_cache_t *anchor;
    list_node_t *head;
    list_node_t *tail; 
} list_t;

static list_node_t *new_list_node(void *value, lcu_generic_callback cleanup_func);
static list_node_t *get_list_node(list_t *handle, int i);
static inline void update_cursor(list_t *handle, int index, list_node_t *node);
static inline void update_node_ptrs(list_node_t *node, list_node_t *prev, list_node_t *next);

lcu_list_t lcu_list_create()
{    
    return lcu_zalloc(sizeof(list_t));
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

    list_node_t *new_node = new_list_node(value, cleanup_func);
    if (new_node == NULL)
        return -1;

    if (list->size == 0) {
        update_node_ptrs(new_node, NULL, NULL);
        list->tail = new_node;
        update_cursor(list, 0, new_node);
    } else {
        update_node_ptrs(new_node, NULL, list->head);
        list->head->prev = new_node;
        if (list->cursor.node)
            list->cursor.index++;
    }
    list->head = new_node;
    list->size++;

    return 0;
}

int lcu_list_insert_back(lcu_list_t handle, void *value, lcu_generic_callback cleanup_func)
{
    if (handle == NULL)
        return -1;

    list_t *list = LIST(handle);

    list_node_t *new_node = new_list_node(value, cleanup_func);
    if (new_node == NULL)
        return -1;

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

    return 0;
}

int lcu_list_insert(lcu_list_t handle, int i, void *value, lcu_generic_callback cleanup_func)
{
    if (handle == NULL)
        return -1;

    list_t *list = LIST(handle);

    if (i == 0)
        return lcu_list_insert_front(list, value, cleanup_func);
    if (i == list->size - 1)
        return lcu_list_insert_back(list, value, cleanup_func);

    int cursor_index = list->cursor.index;

    list_node_t *i_next = get_list_node(list, i);
    if (i_next == NULL)
        return -1;

    list_node_t *new_node = new_list_node(value, cleanup_func);
    if (new_node == NULL)
        return -1;

    list_node_t *i_prev = i_next->prev;
    i_next->prev = new_node;
    i_prev->next = new_node;
    update_node_ptrs(new_node, i_prev, i_next);

    if (i <= cursor_index)
        list->cursor.index++;

    list->size++;

    return 0;
}

void *lcu_list_peek_front(lcu_list_t handle)
{
    if (handle == NULL)
        return NULL;
    return (LIST(handle)->head == NULL ? NULL : LIST(handle)->head->value);
}

void *lcu_list_peek_back(lcu_list_t handle)
{
    if (handle == NULL)
        return NULL;
    return (LIST(handle)->tail == NULL ? NULL : LIST(handle)->tail->value);
}

void *lcu_list_peek(lcu_list_t handle, int i)
{
    if (LIST(handle) == NULL ||
        LIST(handle)->size == 0)
        return NULL;

    list_t *list = LIST(handle);

    if (i == 0)
        return lcu_list_peek_front(list);
    if (i == list->size - 1)
        return lcu_list_peek_back(list);

    list_node_t *list_node = get_list_node(list, i);
    if (list_node == NULL)
        return NULL;

    return list_node->value;
}

int lcu_list_move_to_front(lcu_list_t handle, int i)
{
    if (handle == NULL)
        return -1;

    if (i == 0)
        return 0;

    list_t *list = LIST(handle);
    list_node_t *node;

    if (i == list->size - 1) {
        node = list->tail;
        node->prev->next = NULL;
        list->tail = node->prev;
    } else {
        node = get_list_node(list, i);
        if (node == NULL)
            return -1;
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    update_cursor(list, i, node->prev);
    node->prev = NULL;
    node->next = list->head;
    list->head->prev = node;
    list->head = node;

    return 0;
}

int lcu_list_move_to_back(lcu_list_t handle, int i)
{
    if (handle == NULL)
        return -1;

    if (i == LIST(handle)->size - 1)
        return 0;
    
    list_t *list = LIST(handle);
    list_node_t *node;
    
    if (i == 0) {
        node = list->head;
        node->next->prev = NULL;
        list->head = node->next;
    } else {
        node = get_list_node(list, i);
        if (node == NULL)
            return -1;
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    update_cursor(list, i, node->next);
    node->next = NULL;
    node->prev = list->tail;
    list->tail->next = node;
    list->tail = node;

    return 0;
}

int lcu_list_move(lcu_list_t handle, int from, int to)
{
    if (handle == NULL)
        return -1;

    if (from == to)
        return 0;

    if (to == 0)
        return lcu_list_move_to_front(handle, from);
    if (to == LIST(handle)->size - 1)
        return lcu_list_move_to_back(handle, from);

    list_t *list = LIST(handle);
    list_node_t *node;

    if (from == 0) {
        node = list->head;
        node->next->prev = NULL;
        list->head = node->next;
    } else if (from == list->size - 1) {
        node = list->tail;
        node->prev->next = NULL;
        list->tail = node->prev;
    } else {
        node = get_list_node(list, from);
        if (node == NULL)
            return -1;
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    list_node_t *dst_node = get_list_node(list, to);
    if (dst_node == NULL)
        return -1;

    if (from > to)
        update_cursor(list, from, node->prev);
    else
        update_cursor(list, from, node->next);

    node->next = dst_node;
    node->prev = dst_node->prev;
    dst_node->prev->next = node;
    dst_node->prev = node;
    
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

    list_node_t *list_node = list->head;

    if (list->cursor.node == list_node)
        update_cursor(list, 0, list_node->next);
    else if (list->cursor.index > 0)
        list->cursor.index--;

    if (list_node->cleanup_func != NULL)
        (*list_node->cleanup_func)(list_node->value);

    if (list->size == 1) {
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->head = list_node->next;
        list->head->prev = NULL;
    }
    lcu_free(list_node);
    list->size--;

    return 0;
}

int lcu_list_remove_back(lcu_list_t handle)
{    
    if (handle == NULL || 
        LIST(handle)->size == 0)
        return -1;

    list_t *list = LIST(handle);

    list_node_t *list_node = list->tail;

    if (list->cursor.node == list_node)
        update_cursor(list, list->cursor.index - 1, list_node->prev);

    if (list_node->cleanup_func != NULL)
        (*list_node->cleanup_func)(list_node->value);

    if (list->size == 1) {
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list_node->prev;
        list->tail->next = NULL;

    }
    lcu_free(list_node);
    list->size--;

    return 0;
}

int lcu_list_remove(lcu_list_t handle, int i)
{
    if (LIST(handle) == NULL || 
        LIST(handle)->size == 0)
        return -1;

    if (i == 0)
        return lcu_list_remove_front(handle);
    if (i == LIST(handle)->size - 1)
        return lcu_list_remove_back(handle);

    list_node_t *list_node = get_list_node(LIST(handle), i);
    if (list_node == NULL)
        return -1;

    if (list_node->cleanup_func != NULL)
        (*list_node->cleanup_func)(list_node->value);

    list_node->prev->next = list_node->next;
    list_node->next->prev = list_node->prev;

    if (list_node->next)
        update_cursor(handle, i + 1, list_node->next);
    else
        update_cursor(handle, i - 1, list_node->prev);

    lcu_free(list_node);
    LIST(handle)->size--;

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

    lcu_free(list);
    *handle = NULL;
}

list_node_t *new_list_node(void *value, lcu_generic_callback cleanup_func)
{
    list_node_t *new_node = lcu_zalloc(sizeof(list_node_t));
    if (new_node != NULL) {
        new_node->value = value;
        new_node->cleanup_func = cleanup_func;
    }
    return new_node;
}

list_node_t *get_list_node(list_t *handle, int i)
{
    if (i < 0 || i >= handle->size)
        return NULL;

    list_node_t *node = NULL;
    int current_index = 0;
    if (handle->cursor.node != NULL && 
        abs(handle->cursor.index - i) < i && 
        abs(handle->cursor.index - i) < (handle->size - i)) {
        
        node = handle->cursor.node;
        current_index = handle->cursor.index;

        if (i > current_index) {
            while (current_index < i) {
                node = node->next;
                current_index++;
            }
        } else {
            while (current_index > i) {
                node = node->prev;
                current_index--;
            }
        }
    } else {
        if (i < handle->size / 2) {
            node = handle->head;
            current_index = 0;
            while (current_index < i) {
                node = node->next;
                current_index++;
            }
        } else {
            node = handle->tail;
            current_index = handle->size - 1;
            while (current_index > i) {
                node = node->prev;
                current_index--;
            }
        }
    }
    update_cursor(handle, current_index, node);
    return node;
}

void update_cursor(list_t *handle, int index, list_node_t *node)
{
    handle->cursor.index = index;
    handle->cursor.node = node;
}

void update_node_ptrs(list_node_t *node, list_node_t *prev, list_node_t *next)
{
    node->prev = prev;
    node->next = next;
}
