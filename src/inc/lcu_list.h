#ifndef LCU_LIST_H
#define LCU_LIST_H

#include <stddef.h>
#include "lcu.h"

typedef void * lcu_list_t;

lcu_list_t lcu_list_create(size_t max_hint);
size_t lcu_list_get_size(lcu_list_t handle);
int lcu_list_insert_front(lcu_list_t handle, void *value, lcu_generic_callback cleanup_func);
int lcu_list_insert_back(lcu_list_t handle, void *value, lcu_generic_callback cleanup_func);
int lcu_list_insert(lcu_list_t handle, int i, void *value, lcu_generic_callback cleanup_func);
void *lcu_list_peek_front(lcu_list_t handle);
void *lcu_list_peek_back(lcu_list_t handle);
void *lcu_list_peek(lcu_list_t handle, int i);
int lcu_list_move_to_front(lcu_list_t handle, int i);
int lcu_list_move_to_back(lcu_list_t handle, int i);
int lcu_list_move(lcu_list_t handle, int from, int to);
int lcu_list_swap_with_front(lcu_list_t handle, int i);
int lcu_list_swap_with_back(lcu_list_t handle, int i);
int lcu_list_swap(lcu_list_t handle, int i, int j);
int lcu_list_remove_front(lcu_list_t handle);
int lcu_list_remove_back(lcu_list_t handle);
int lcu_list_remove(lcu_list_t handle, int i);
int lcu_list_build_skip_list(lcu_list_t handle);
void lcu_list_destroy(lcu_list_t *handle);

#endif // LCU_LIST_H