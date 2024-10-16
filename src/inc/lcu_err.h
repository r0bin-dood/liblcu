#ifndef LCU_ERR_H
#define LCU_ERR_H

#include "lcu.h"

enum lcu_err {
    LCU_ERR_OK = 0,
};
typedef enum lcu_err lcu_err_t;

const char *err_str_arr[] = {
    "Ok",

};

void lcu_err_ok_or_panic(lcu_err_t err);
bool lcu_err_is_ok(lcu_err_t err);
const char *lcu_err_str(lcu_err_t err);

#endif // LCU_ERR_H