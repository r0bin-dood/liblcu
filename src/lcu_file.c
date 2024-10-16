#include <stdlib.h>
#include "lcu_file.h"

FILE *lcu_file_open(const char *file, const char *mode)
{
    FILE *fd = fopen(file, mode);
    return fd;
}

size_t lcu_file_get_size(FILE *fd)
{
    if (fseek(fd, 0, SEEK_END) != 0) 
        return 0;
    long file_size = ftell(fd);
    if (file_size == -1) 
        file_size = 0;
    fseek(fd, 0, SEEK_SET);

    return (size_t)file_size;
}

int lcu_file_read(FILE *fd, lcu_buf_t *data)
{
    size_t size = lcu_file_get_size(fd);
    return lcu_file_read_nbytes(fd, data, size);
}

int lcu_file_read_nbytes(FILE *fd, lcu_buf_t *data, size_t n)
{
    lcu_buf_create(data, NULL, n);
    fread(data->buf, sizeof(char), n, fd);
    return 0;
}

void lcu_file_close(FILE *fd)
{
    fclose(fd);
}
