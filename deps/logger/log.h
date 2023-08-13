/**
 * Copyright (c) 2020 rxi
 * Copyright (c) 2023 edd255
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define LOG_VERSION "0.2.0"

typedef struct {
    va_list args;
    const char* fmt;
    const char* fn;
    const char* file;
    struct tm* time;
    void* udata;
    int line;
    int level;
} log_event_t;

typedef void (*logc_logfn_t)(log_event_t* ev);
typedef void (*logc_lockfn_t)(bool lock, void* udata);

enum { LOGC_TRACE, LOGC_DEBUG, LOGC_INFO, LOGC_WARN, LOGC_ERROR, LOGC_FATAL };

#define log_trace(...) log_log(LOGC_TRACE, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOGC_DEBUG, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOGC_INFO, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOGC_WARN, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOGC_ERROR, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOGC_FATAL, __func__, __FILE__, __LINE__, __VA_ARGS__)

const char* log_level_string(int level);
void log_set_lock(logc_lockfn_t fn, void* udata);
void log_set_level(int level);
void log_set_quiet(bool enable);
int log_add_callback(logc_logfn_t fn, void* udata, int level);
int log_add_fp(FILE* fp, int level);

void log_log(int level, const char* fn, const char* file, int line, const char* fmt, ...);

#endif
