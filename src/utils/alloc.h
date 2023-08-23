#pragma once

#ifndef LISPY_UTILS_ALLOC_H
#define LISPY_UTILS_ALLOC_H

#include <stddef.h>

void* log_malloc(size_t size, const char* fn, const char* file, int line);
void* log_realloc(
    void* old_ptr,
    size_t size,
    const char* fn,
    const char* file,
    int line
);
void log_free(void* ptr, const char* fn, const char* file, int line);

#ifdef LOG_ALLOCS
#define MALLOC(size) log_malloc((size), __func__, __FILE__, __LINE__)
#define REALLOC(old_ptr, size) \
    log_realloc((old_ptr), (size), __func__, __FILE__, __LINE__)
#define FREE(ptr) log_free((ptr), __func__, __FILE__, __LINE__)
#else
#define MALLOC(size)           malloc((size))
#define REALLOC(old_ptr, size) realloc((old_ptr), (size))
#define FREE(ptr)              free((ptr))
#endif

#endif
