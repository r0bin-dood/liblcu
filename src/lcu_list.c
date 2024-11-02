#include <string.h>
#include <stdlib.h>
#include "lcu_list.h"

#define LIST(x) ((list_t *)x)

/**
 * \internal
 * This struct is used internally.
 */
typedef struct list_node {
    size_t size;
    void *value;
    struct list_node *prev;
    struct list_node *next;
} list_node_t;

/**
 * \internal
 * This struct is used internally.
 */
typedef struct list {
    size_t size;
    list_node_t *head;
    list_node_t *tail;
    lcu_generic_callback cleanup_func;
} list_t;

static list_node_t *new_list_node(void *value);
static list_node_t *get_list_node(list_t *handle, int i);
static inline void update_node_ptrs(list_node_t *node, list_node_t *prev, list_node_t *next);
static int list_insert(bool from_back, list_t *handle, void *value);
static void *list_peek(bool from_back, list_t *handle);
static int list_remove(bool from_back, list_t *handle);

lcu_list_t lcu_list_create(lcu_generic_callback cleanup_func)
{
    list_t *handle = calloc(1, sizeof(list_t));
    if (handle != NULL)
        handle->cleanup_func = cleanup_func;

    return handle;
}

size_t lcu_list_get_size(lcu_list_t handle)
{
    if (handle == NULL)
        return 0;
    return LIST(handle)->size;
}

int lcu_list_insert_front(lcu_list_t handle, void *value)
{
    return list_insert(false, handle, value);
}

int lcu_list_insert_back(lcu_list_t handle, void *value)
{
    return list_insert(true, handle, value);
}

int lcu_list_insert_at_i(lcu_list_t handle, int i, void *value)
{
    if (handle == NULL)
        return -1;

    if (i == 0)
        return lcu_list_insert_front(handle, value);
    if (i < 0 || i >= (int)(LIST(handle)->size))
        return lcu_list_insert_back(handle, value);

    list_node_t *i_next = get_list_node(handle, i);
    if (i_next == NULL)
        return -1;

    list_node_t *new_node = new_list_node(value);
    if (new_node == NULL)
        return -1;

    list_node_t *i_prev = i_next->prev;
    i_next->prev = new_node;
    i_prev->next = new_node;
    new_node->next = i_next;
    new_node->prev = i_prev;

    LIST(handle)->size++;

    return 0;
}

void *lcu_list_peek_front(lcu_list_t handle)
{
    return list_peek(false, handle);
}

void *lcu_list_peek_back(lcu_list_t handle)
{
    return list_peek(true, handle);
}

void *lcu_list_peek_at_i(lcu_list_t handle, int i)
{
    if (LIST(handle) == NULL ||
        LIST(handle)->size == 0)
        return NULL;

    if (i == 0)
        return lcu_list_peek_front(handle);
    if (i < 0 || i >= (int)(LIST(handle)->size))
        return lcu_list_peek_back(handle);

    list_node_t *list_node = get_list_node(LIST(handle), i);
    if (list_node == NULL)
        return NULL;

    return list_node->value;
}

int lcu_list_remove_front(lcu_list_t handle)
{
    return list_remove(false, handle);
}

int lcu_list_remove_back(lcu_list_t handle)
{    
    return list_remove(true, handle);
}

int lcu_list_remove_at_i(lcu_list_t handle, int i)
{
    if (LIST(handle) == NULL || 
        LIST(handle)->size == 0)
        return -1;

    if (i == 0)
        return lcu_list_remove_front(handle);
    if (i < 0 || i >= (int)(LIST(handle)->size))
        return lcu_list_remove_back(handle);

    list_node_t *list_node = get_list_node(LIST(handle), i);
    if (list_node == NULL)
        return -1;

    if (LIST(handle)->cleanup_func != NULL)
        (*LIST(handle)->cleanup_func)(list_node->value);

    list_node->prev->next = list_node->next;
    list_node->next->prev = list_node->prev;

    free(list_node);
    LIST(handle)->size--;

    return 0;
}

void lcu_list_destroy(lcu_list_t *handle)
{
    if (handle == NULL || 
        *handle == NULL)
        return;

    list_t *list = (list_t *)*handle;
    size_t list_size = list->size;
    for (size_t i = 0; i < list_size; i++)
        lcu_list_remove_back(list);

    free(list);
    *handle = NULL;
}

list_node_t *new_list_node(void *value)
{
    list_node_t *new_node = calloc(1, sizeof(list_node_t));
    if (new_node != NULL)
        new_node->value = value;

    return new_node;
}

list_node_t *get_list_node(list_t *handle, int i)
{
    if (i < 0 || i >= handle->size)
        return NULL;

    list_node_t *list_node = handle->head;
    while (i--)
        list_node = list_node->next;

    return list_node;
}

int list_insert(bool from_back, list_t *handle, void *value)
{
    if (handle == NULL)
        return -1;

    list_node_t *new_node = new_list_node(value);
    if (new_node == NULL)
        return -1;

    if (handle->size == 0) {
        update_node_ptrs(new_node, NULL, NULL);
        if (from_back)
            handle->head = new_node;
        else
            handle->tail = new_node;
    } else {
        if (from_back) {
            update_node_ptrs(new_node, handle->tail, NULL);
            handle->tail->next = new_node;
        } else {
            update_node_ptrs(new_node, NULL, handle->head);
            handle->head->prev = new_node;
        }
    }
    if (from_back)
        handle->tail = new_node;
    else
        handle->head = new_node;
    handle->size++;

    return 0;
}

void *list_peek(bool from_back, list_t *handle)
{
    if (handle == NULL)
        return NULL;
    if (from_back) {
        if (handle->tail == NULL)
            return NULL;
    } else {
        if (handle->head == NULL)
            return NULL;
    }
    return (from_back ? handle->tail->value : handle->head->value);
}

int list_remove(bool from_back, list_t *handle)
{
    if (handle == NULL || 
        handle->size == 0)
        return -1;

    list_node_t *list_node = from_back ? handle->tail : handle->head;

    if (handle->cleanup_func != NULL)
        (*handle->cleanup_func)(list_node->value);

    if (handle->size == 1) {
        handle->head = NULL;
        handle->tail = NULL;
    } else {
        if (from_back) {
            handle->tail = list_node->prev;
            handle->tail->next = NULL;
        } else {
            handle->head = list_node->next;
            handle->head->prev = NULL;
        }
    }
    free(list_node);
    handle->size--;

    return 0;
}

void update_node_ptrs(list_node_t *node, list_node_t *prev, list_node_t *next)
{
    node->prev = prev;
    node->next = next;
}
