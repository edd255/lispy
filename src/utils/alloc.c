#include "alloc.h"

#include "common.h"
#include "deps/logger/log.h"

//=== LOGGING MEMORY ALLOCATIONS ===============================================
void* log_malloc(size_t size, const char* fn, const char* file, int line) {
    void* ptr = malloc(size);
    log_log(LOGC_TRACE, fn, file, line, "Address: %p", ptr);
    return ptr;
}

void* log_realloc(
    void* old_ptr,
    size_t size,
    const char* fn,
    const char* file,
    int line
) {
    void* new_ptr = realloc(old_ptr, size);
    log_log(
        LOGC_TRACE,
        fn,
        file,
        line,
        "Old address: %p. New Address: %p",
        old_ptr,
        new_ptr
    );
    return new_ptr;
}

void log_free(void* ptr, const char* fn, const char* file, int line) {
    log_log(LOGC_TRACE, fn, file, line, "Freeing: %p", ptr);
    free(ptr);
}
