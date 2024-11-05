#ifndef LCU_H
#define LCU_H

#include "lcu_alloc.h"

typedef int bool;
#define false (0)
#define true  (1)

#define stringify(x) #x

#define UNUSED(x) ((void)(x))

typedef void (*lcu_generic_callback)(void *);

#endif // LCU_H