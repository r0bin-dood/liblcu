#ifndef LCU_ERR_H
#define LCU_ERR_H

#include "lcu.h"

typedef enum lcu_err {
    LCU_ERR_OK = 0,
    LCU_ERR_ERRNO = -1,
    LCU_ERR_ERROR = -__INT8_MAX__,
    LCU_ERR_INVAL,
    LCU_ERR_ALLOC,
    LCU_ERR_OVERFLOW,
} lcu_err_t;

void lcu_err_ok_or_panic(lcu_err_t err);
bool lcu_err_ok(lcu_err_t err);
const char *lcu_err_str(lcu_err_t err);

#endif // LCU_ERR_H