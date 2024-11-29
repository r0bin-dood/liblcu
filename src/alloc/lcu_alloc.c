#include <stdlib.h>
#include <string.h>
#include "lcu_alloc.h"

void *lcu_alloc(size_t size)
{
    return malloc(size);
}

void *lcu_zalloc(size_t size)
{
    return calloc(1, size);
}

void lcu_free(void *buf)
{
    if (buf != NULL)
        free(buf);
}