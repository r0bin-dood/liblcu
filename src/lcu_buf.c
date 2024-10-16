#include <stdlib.h>
#include <string.h>
#include "lcu_buf.h"

void lcu_buf_init(lcu_buf_t *new_buf, void *data, size_t size)
{
    if (data == NULL)
        return;
    
    new_buf->buf = data;
    new_buf->size = size;
    new_buf->alloc = false;
}

void lcu_buf_create(lcu_buf_t *new_buf, const void *data, size_t size)
{
    if (size == 0)
        return;

    new_buf->size = size;
    new_buf->alloc = true;
    new_buf->buf = malloc(new_buf->size);
    if (data != NULL)
        memcpy(new_buf->buf, data, new_buf->size);
    else
        bzero(new_buf->buf, new_buf->size);

}

int lcu_buf_compare(const lcu_buf_t *b1, const lcu_buf_t *b2)
{
    size_t min_size = b1->size < b2->size ? b1->size : b2->size;
    int res = memcmp(b1->buf, b2->buf, min_size);
    if (res != 0)
        return res;
    return (b1->size > b2->size) - (b1->size < b2->size);
}

void lcu_buf_concat(lcu_buf_t *out, const lcu_buf_t *b1, const lcu_buf_t *b2)
{
    size_t size = b1->size + b2->size + 1;

    lcu_buf_create(out, NULL, size);
    
    memcpy(out->buf, b1->buf, b1->size);
    memcpy((char *)out->buf + b1->size, b2->buf, b2->size);
}

void lcu_buf_destroy(lcu_buf_t *buf)
{
    if (buf->alloc == true)
        free(buf->buf);
    buf->buf = NULL;
    buf->size = 0;
    buf->alloc = false;
}
