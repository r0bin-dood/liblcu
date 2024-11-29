#include <stdlib.h>
#include <assert.h>
#include "lcu_err.h"

static const char *err_str_arr[] = {
    "Ok",
    "Error",
    "Error",
    "Invalid Argument",
    "Allocation Failure",
};

void lcu_err_ok_or_panic(lcu_err_t err)
{
    assert(err == LCU_ERR_OK);
}

bool lcu_err_ok(lcu_err_t err)
{
    return ((err == LCU_ERR_OK) ? true : false);
}

const char *lcu_err_str(lcu_err_t err)
{
    return err_str_arr[abs(err)];
}