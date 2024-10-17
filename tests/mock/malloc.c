#define _GNU_SOURCE
#include <stdlib.h>
#include <dlfcn.h>

int force_malloc_failure = 0;

void *malloc(size_t size) {
    if (force_malloc_failure)
        return NULL;

    void* (*original_malloc)(size_t) = dlsym(RTLD_NEXT, "malloc");

    if (original_malloc == NULL)
        exit(EXIT_FAILURE);

    return original_malloc(size);
}