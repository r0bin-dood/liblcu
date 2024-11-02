#define _GNU_SOURCE
#include <stdlib.h>
#include <dlfcn.h>

int force_malloc_failure = 0;

void *malloc(size_t size)
{
    if (force_malloc_failure)
        return NULL;

    void* (*original_malloc)(size_t) = dlsym(RTLD_NEXT, "malloc");

    if (original_malloc == NULL)
        exit(EXIT_FAILURE);

    return original_malloc(size);
}

void *calloc(size_t nmemb, size_t size)
{
    if (force_malloc_failure)
        return NULL;

    void* (*original_calloc)(size_t, size_t) = dlsym(RTLD_NEXT, "calloc");

    if (original_calloc == NULL)
        exit(EXIT_FAILURE);

    return original_calloc(nmemb, size);
}