#include <stdlib.h>
#include "lcu_file.h"

FILE *lcu_file_open(const char *file, const char *mode)
{
    if (file == NULL || mode == NULL)
        return NULL;
    return fopen(file, mode);
}

size_t lcu_file_get_size(FILE *fd)
{
    if (fd == NULL)
        return 0;
    if (fseek(fd, 0, SEEK_END) != 0) 
        return 0;
    long file_size = ftell(fd);
    if (file_size == -1) 
        file_size = 0;
    fseek(fd, 0, SEEK_SET);

    return (size_t)file_size;
}

int lcu_file_write(FILE *fd, lcu_buf_t *data)
{
    if (fd == NULL || data == NULL)
        return -1;
    size_t bytes_written = fwrite(data->buf, sizeof(char), data->size, fd);
    if (bytes_written < data->size)
        return -1;
    if (fflush(fd) != 0)
        return -1;
    return 0;
}

int lcu_file_read(FILE *fd, lcu_buf_t *data)
{
    if (fd == NULL || data == NULL)
        return -1;
    size_t size = lcu_file_get_size(fd);
    return lcu_file_read_nbytes(fd, data, size);
}

int lcu_file_read_nbytes(FILE *fd, lcu_buf_t *data, size_t n)
{
    if (fd == NULL || data == NULL)
        return -1;
    lcu_buf_create(data, NULL, n);
    fread(data->buf, sizeof(char), n, fd);
    return 0;
}

int lcu_file_close(FILE *fd)
{
    if (fd == NULL)
        return -1;
    return fclose(fd);
}
