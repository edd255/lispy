/// @file alloc.h
/// @brief This file contains methods to log memory management for debugging
/// purposes

#pragma once

#ifndef LISPY_UTILS_ALLOC_H
#define LISPY_UTILS_ALLOC_H

#include <stddef.h>

/// @brief Allocates memory and logs function, file, line and pointer.
/// @param size The requested memory size
/// @param fn The caller of the function
/// @param file The file in which the calling function resides
/// @param line The line in which the calling function resides
void* log_malloc(size_t size, const char* fn, const char* file, int line);

/// @brief Reallocates memory and logs function, file, line, old and new pointer.
/// @param old_ptr A pointer to the memory region which should be reallocated
/// @param size The requested memory size
/// @param fn The caller of the function
/// @param file The file in which the calling function resides
/// @param line The line in which the calling function resides
void* log_realloc(
    void* old_ptr,
    size_t size,
    const char* fn,
    const char* file,
    int line
);

/// @brief Frees memory and logs function, file, line, and pointer.
/// @param ptr The pointer that should be freed
/// @param fn The caller of the function
/// @param file The file in which the calling function resides
/// @param line The line in which the calling function resides
void log_free(void* ptr, const char* fn, const char* file, int line);

#ifdef LOG_ALLOCS
/// malloc with additional debug logging
#define MALLOC(size) log_malloc((size), __func__, __FILE__, __LINE__)
/// realloc with additional debug logging
#define REALLOC(old_ptr, size) \
    log_realloc((old_ptr), (size), __func__, __FILE__, __LINE__)
/// free with additional debug logging
#define FREE(ptr) log_free((ptr), __func__, __FILE__, __LINE__)
#else
/// malloc without logging the procedure
#define MALLOC(size)           malloc((size))
/// realloc without logging the procedure
#define REALLOC(old_ptr, size) realloc((old_ptr), (size))
/// free without logging the procedure
#define FREE(ptr)              free((ptr))
#endif

#endif
