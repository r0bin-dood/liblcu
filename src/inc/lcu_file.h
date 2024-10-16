#ifndef LCU_FILE_H
#define LCU_FILE_H

#include <stdio.h>
#include <stddef.h>
#include "lcu_buf.h"

FILE *lcu_file_open(const char *file, const char *mode);
size_t lcu_file_get_size(FILE *fd);
int lcu_file_read(FILE *fd, lcu_buf_t *data);
int lcu_file_read_nbytes(FILE *fd, lcu_buf_t *data, size_t n);
void lcu_file_close(FILE *fd);

#endif // LCU_FILE_H