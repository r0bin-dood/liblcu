#include <string.h>
#include <stdlib.h>
#include "lcu_list.h"

#define LIST(x) ((list_t *)x)

/**
 * \internal
 * This struct is used internally.
 */
typedef struct list_item {
    size_t size;
    void *value;
    struct list_item *prev;
    struct list_item *next;
} list_item_t;

/**
 * \internal
 * This struct is used internally.
 */
typedef struct list {
    size_t size;
    list_item_t *head;
    list_item_t *tail;
    lcu_generic_callback cleanup_func;
} list_t;

static inline list_item_t *new_list_item(void *value);
static inline list_item_t *get_list_item(list_t *handle, int i);
static inline void update_node_ptrs(list_item_t *node, list_item_t *prev, list_item_t *next);
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
    if (LIST(handle) == NULL)
        return 0;
    return LIST(handle)->size;
}

int lcu_list_insert_front(lcu_list_t handle, void *value)
{
    if (LIST(handle) == NULL)
        return -1;

    list_item_t *temp = new_list_item(value);
    if (temp == NULL)
        return -1;

    if (LIST(handle)->size == 0)
    {
        update_node_ptrs(temp, NULL, NULL);
        LIST(handle)->tail = temp;
    }
    else
    {
        update_node_ptrs(temp, NULL, LIST(handle)->head);
        LIST(handle)->head->prev = temp;
    }
    LIST(handle)->head = temp;
    LIST(handle)->size++;

    return 0;
}

int lcu_list_insert_back(lcu_list_t handle, void *value)
{
    if (LIST(handle) == NULL)
        return -1;

    list_item_t *temp = new_list_item(value);
    if (temp == NULL)
        return -1;

    if (LIST(handle)->size == 0)
    {
        update_node_ptrs(temp, NULL, NULL);
        LIST(handle)->head = temp;
    }
    else
    {
        update_node_ptrs(temp, LIST(handle)->tail, NULL);
        LIST(handle)->tail->next = temp;
    }
    LIST(handle)->tail = temp;
    LIST(handle)->size++;

    return 0;
}

int lcu_list_insert_at_i(lcu_list_t handle, int i, void *value)
{
    if (LIST(handle) == NULL)
        return -1;

    if (i == 0)
        return lcu_list_insert_front(handle, value);
    if (i < 0 || i >= (int)(LIST(handle)->size))
        return lcu_list_insert_back(handle, value);

    list_item_t *i_next = get_list_item(LIST(handle), i);
    if (i_next == NULL)
        return -1;

    list_item_t *new_item = new_list_item(value);
    if (new_item == NULL)
        return -1;

    list_item_t *i_prev = i_next->prev;
    i_next->prev = new_item;
    i_prev->next = new_item;
    new_item->next = i_next;
    new_item->prev = i_prev;

    LIST(handle)->size++;

    return 0;
}

void *lcu_list_peek_front(lcu_list_t handle)
{
    if (LIST(handle) == NULL ||
        LIST(handle)->head == NULL)
        return NULL;
    return (LIST(handle)->head->value);
}

void *lcu_list_peek_back(lcu_list_t handle)
{
    if (LIST(handle) == NULL ||
        LIST(handle)->tail == NULL)
        return NULL;
    return (LIST(handle)->tail->value);
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

    list_item_t *temp = get_list_item(LIST(handle), i);
    if (temp == NULL)
        return NULL;

    return temp->value;
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

    list_item_t *temp = get_list_item(LIST(handle), i);
    if (temp == NULL)
        return -1;

    if (LIST(handle)->cleanup_func != NULL)
        (*LIST(handle)->cleanup_func)(temp->value);

    temp->prev->next = temp->next;
    temp->next->prev = temp->prev;

    free(temp);
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

list_item_t *new_list_item(void *value)
{
    list_item_t *temp = calloc(1, sizeof(list_item_t));
    if (temp != NULL)
        temp->value = value;

    return temp;
}

list_item_t *get_list_item(list_t *handle, int i)
{
    if (i < 0 || i >= handle->size)
        return NULL;

    list_item_t *temp = handle->head;
    while (i--)
        temp = temp->next;

    return temp;
}

int list_remove(bool from_back, list_t *handle)
{
    if (handle == NULL || 
        handle->size == 0)
        return -1;

    list_item_t *temp = from_back ? handle->tail : handle->head;

    if (handle->cleanup_func != NULL)
        (*handle->cleanup_func)(temp->value);

    if (handle->size == 1)
    {
        handle->head = NULL;
        handle->tail = NULL;
    }
    else
    {
        if (from_back)
        {
            handle->tail = temp->prev;
            handle->tail->next = NULL;
        }
        else
        {
            handle->head = temp->next;
            handle->head->prev = NULL;
        }
    }
    free(temp);
    handle->size--;

    return 0;
}

void update_node_ptrs(list_item_t *node, list_item_t *prev, list_item_t *next)
{
    node->prev = prev;
    node->next = next;
}
