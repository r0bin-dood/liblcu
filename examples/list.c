#include "main.h"

struct a {
    int foo;
    char bar;
};

void example_linked_list()
{    
    lcu_logger_print("%s():\n", __func__);

    lcu_list_t ll = lcu_list_create(NULL);

    int x = 1337; 
    char *hw = "Hello, World!";
    lcu_list_insert_front(ll, &x);
    lcu_list_insert_back(ll, hw);
    lcu_logger_print("nodes: %lu (insert 2 nodes one an 'int' the other a 'char *')\n", lcu_list_get_size(ll));
    for (int i = 0; (size_t) i < lcu_list_get_size(ll); i++)
    {
        if (i == 0)
            lcu_logger_print("node[%d]: %d\n", i, *((int *)lcu_list_peek_front(ll)));
        else
            lcu_logger_print("node[%d]: %s\n", i, (char *)lcu_list_peek_at_i(ll, i));
    }
    struct a a_st = {
        .foo = 33,
        .bar = '!'
    };
    lcu_list_insert_at_i(ll, 1, &a_st);
    lcu_logger_print("nodes: %lu (at pos 1 insert a 'struct a')\n", lcu_list_get_size(ll));
    struct a *a_p = lcu_list_peek_at_i(ll, 1);
    lcu_logger_print("node[0]: %d\n", *((int *)lcu_list_peek_front(ll)));
    lcu_logger_print("node[1]: %d %c\n", a_p->foo, a_p->bar);
    lcu_logger_print("node[2]: %s\n", (char *)lcu_list_peek_back(ll));
    lcu_list_remove_front(ll);
    lcu_logger_print("nodes: %lu (pop the front) \n", lcu_list_get_size(ll));
    a_p = lcu_list_peek_front(ll);
    lcu_logger_print("node[0]: %d %c\n", a_p->foo, a_p->bar);

    lcu_list_destroy(&ll);

    lcu_logger_print("\n\n");
}