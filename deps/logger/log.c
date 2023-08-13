/*
 * Copyright (c) 2020 rxi
 * Copyright (c) 2023 edd255
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "log.h"

#define MAX_CALLBACKS 32

typedef struct {
    logc_logfn_t fn;
    void* udata;
    int level;
} callback_t;

static struct {
    void* udata;
    logc_lockfn_t lock;
    int level;
    bool quiet;
    callback_t callbacks[MAX_CALLBACKS];
} logc_logger_t;

static const char* level_strings[] =
    {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

#ifdef LOGC_USE_COLOR
static const char* level_colors[] =
    {"\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"};
#endif

static void stdout_callback(log_event_t* event) {
    char buf[16];
    buf[strftime(buf, sizeof(buf), "%H:%M:%S", event->time)] = '\0';
#ifdef LOGC_USE_COLOR
    fprintf(
        event->udata,
        "%s \x1b[1m%s%-5s\x1b[0m %s:%d in %s - ",
        buf,
        level_colors[event->level],
        level_strings[event->level],
        event->file,
        event->line,
        event->fn
    );
#else
    fprintf(
        event->udata,
        "%s %-5s %s:%d in %s - ",
        buf,
        level_strings[event->level],
        event->file,
        event->line,
        event->fn
    );
#endif
    vfprintf(event->udata, event->fmt, event->args);
    fprintf(event->udata, "\n");
    fflush(event->udata);
}

static void file_callback(log_event_t* event) {
    char buf[64];
    buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", event->time)] = '\0';
    fprintf(
        event->udata,
        "%s %-5s %s:%d in %s - ",
        buf,
        level_strings[event->level],
        event->file,
        event->line,
        event->fn
    );
    vfprintf(event->udata, event->fmt, event->args);
    fprintf(event->udata, "\n");
    fflush(event->udata);
}

static void lock(void) {
    if (logc_logger_t.lock) {
        logc_logger_t.lock(true, logc_logger_t.udata);
    }
}

static void unlock(void) {
    if (logc_logger_t.lock) {
        logc_logger_t.lock(false, logc_logger_t.udata);
    }
}

const char* log_level_string(int level) {
    return level_strings[level];
}

void log_set_lock(logc_lockfn_t fn, void* udata) {
    logc_logger_t.lock = fn;
    logc_logger_t.udata = udata;
}

void log_set_level(int level) {
    logc_logger_t.level = level;
}

void log_set_quiet(bool enable) {
    logc_logger_t.quiet = enable;
}

int log_add_callback(logc_logfn_t fn, void* udata, int level) {
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        if (!logc_logger_t.callbacks[i].fn) {
            logc_logger_t.callbacks[i] = (callback_t) {fn, udata, level};
            return 0;
        }
    }
    return -1;
}

int log_add_fp(FILE* fp, int level) {
    return log_add_callback(file_callback, fp, level);
}

static void init_event(log_event_t* event, void* udata) {
    if (!event->time) {
        time_t t = time(NULL);
        event->time = localtime(&t);
    }
    event->udata = udata;
}

void log_log(int level, const char* fn, const char* file, int line, const char* fmt, ...) {
    log_event_t event = {
        .fmt = fmt,
        .fn = fn,
        .file = file,
        .line = line,
        .level = level,
    };
    lock();
    if (!logc_logger_t.quiet && level >= logc_logger_t.level) {
        init_event(&event, stderr);
        va_start(event.args, fmt);
        stdout_callback(&event);
        va_end(event.args);
    }
    for (int i = 0; i < MAX_CALLBACKS && logc_logger_t.callbacks[i].fn; i++) {
        callback_t* cb = &logc_logger_t.callbacks[i];
        if (level >= cb->level) {
            init_event(&event, cb->udata);
            va_start(event.args, fmt);
            cb->fn(&event);
            va_end(event.args);
        }
    }
    unlock();
}
