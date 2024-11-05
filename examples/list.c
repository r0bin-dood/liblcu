#include "main.h"
#include "unistd.h"

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
    lcu_list_insert_front(ll, &x, NULL);
    lcu_list_insert_back(ll, hw, NULL);
    lcu_logger_print("nodes: %lu (insert 2 nodes one an 'int' the other a 'char *')\n", lcu_list_get_size(ll));
    for (int i = 0; (size_t) i < lcu_list_get_size(ll); i++)
    {
        if (i == 0)
            lcu_logger_print("node[%d]: %d\n", i, *((int *)lcu_list_peek_front(ll)));
        else
            lcu_logger_print("node[%d]: %s\n", i, (char *)lcu_list_peek(ll, i));
    }
    struct a a_st = {
        .foo = 33,
        .bar = '!'
    };
    lcu_list_insert(ll, 1, &a_st, NULL); // since the list has 2 items (0 and 1), using size - 1 as index is the same as calling lcu_list_insert_back
    lcu_logger_print("nodes: %lu (at pos 1 insert a 'struct a')\n", lcu_list_get_size(ll));
    struct a *a_p = lcu_list_peek_back(ll);
    lcu_logger_print("node[0]: %d\n", *((int *)lcu_list_peek_front(ll)));
    lcu_logger_print("node[1]: %s\n", (char *)lcu_list_peek(ll, 1));
    lcu_logger_print("node[2]: %d %c\n", a_p->foo, a_p->bar);
    // gotta wait or the print won't happen fast enough
    usleep(10000);
    lcu_list_swap_with_back(ll, 1);
    lcu_list_remove_front(ll);
    lcu_logger_print("nodes: %lu (pop the front) \n", lcu_list_get_size(ll));
    a_p = lcu_list_peek_front(ll);
    lcu_logger_print("node[0]: %d %c\n", a_p->foo, a_p->bar);

    lcu_list_destroy(&ll);

    lcu_logger_print("\n\n");
}