#include "color-sh.h"

#include <stdarg.h>
#include <stdio.h>

#if HAS_WIN
    void set_color(int color) {
        if (!out) {
            out = GetStdHandle(STD_OUTPUT_HANDLE);
        }
        SetConsoleTextAttribute(out, FOREGROUND_INTENSITY | color);
    }
#elif HAS_NIX
    void set_color(char* color) {
        printf("%s", color);
    }
#else
    #error Platform not supported by color-sh lib
#endif

void color_red(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    set_color(RED);
    vprintf(fmt, ap);
    va_end(ap);
    set_color(REWIND);
}

void color_green(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    set_color(GREEN);
    vprintf(fmt, ap);
    va_end(ap);
    set_color(REWIND);
}

void color_white(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    set_color(WHITE);
    vprintf(fmt, ap);
    va_end(ap);
    set_color(REWIND);
}

void color_blue(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    set_color(BLUE);
    vprintf(fmt, ap);
    va_end(ap);
    set_color(REWIND);
}

void color_purple(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    set_color(PURPLE);
    vprintf(fmt, ap);
    va_end(ap);
    set_color(REWIND);
}

void color_yellow(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    set_color(YELLOW);
    vprintf(fmt, ap);
    va_end(ap);
    set_color(REWIND);
}

void color_cyan(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    set_color(CYAN);
    vprintf(fmt, ap);
    va_end(ap);
    set_color(REWIND);
}

void color_bred(char* fmt, ...) {
    #if HAS_NIX
        va_list ap;
        va_start(ap, fmt);
        set_color(RED_BOLD);
        vprintf(fmt, ap);
        va_end(ap);
        set_color(REWIND);
    #elif HAS_WIN
        va_list ap;
        va_start(ap, fmt);
        set_color(RED);
        vprintf(fmt, ap);
        va_end(ap);
        set_color(REWIND);
    #else
        #error Platform not supported by color-sh lib
    #endif
}

void color_bgreen(char* fmt, ...) {
    #if HAS_NIX
        va_list ap;
        va_start(ap, fmt);
        set_color(GREEN_BOLD);
        vprintf(fmt, ap);
        va_end(ap);
        set_color(REWIND);
    #elif HAS_WIN
        va_list ap;
        va_start(ap, fmt);
        set_color(GREEN);
        vprintf(fmt, ap);
        va_end(ap);
        set_color(REWIND);
    #else
        #error Platform not supported by color-sh lib
    #endif
}

void color_bwhite(char* fmt, ...) {
    #if HAS_NIX
        va_list ap;
        va_start(ap, fmt);
        set_color(WHITE_BOLD);
        vprintf(fmt, ap);
        va_end(ap);
        set_color(REWIND);
    #elif HAS_WIN
        va_list ap;
        va_start(ap, fmt);
        set_color(WHITE);
        vprintf(fmt, ap);
        va_end(ap);
        set_color(REWIND);
    #else
        #error Platform not supported by color-sh lib
    #endif
}

void color_bblue(char* fmt, ...) {
    #if HAS_NIX
        va_list ap;
        va_start(ap, fmt);
        set_color(BLUE_BOLD);
        vprintf(fmt, ap);
        va_end(ap);
        set_color(REWIND);
    #elif HAS_WIN
        va_list ap;
        va_start(ap, fmt);
        set_color(BLUE);
        vprintf(fmt, ap);
        va_end(ap);
        set_color(REWIND);
    #else
        #error Platform not supported by color-sh lib
    #endif
}

void color_bpurple(char* fmt, ...) {
    #if HAS_NIX
        va_list ap;
        va_start(ap, fmt);
        set_color(PURPLE_BOLD);
        vprintf(fmt, ap);
        va_end(ap);
        set_color(REWIND);
    #elif HAS_WIN
        va_list ap;
        va_start(ap, fmt);
        set_color(PURPLE);
        vprintf(fmt, ap);
        va_end(ap);
        set_color(REWIND);
    #else
        #error Platform not supported by color-sh lib
    #endif
}

void color_byellow(char* fmt, ...) {
    #if HAS_NIX
        va_list ap;
        va_start(ap, fmt);
        set_color(YELLOW_BOLD);
        vprintf(fmt, ap);
        va_end(ap);
        set_color(REWIND);
    #elif HAS_WIN
        va_list ap;
        va_start(ap, fmt);
        set_color(YELLOW);
        vprintf(fmt, ap);
        va_end(ap);
        set_color(REWIND);
    #else
        #error Platform not supported by color-sh lib
    #endif
}

void color_bcyan(char* fmt, ...) {
    #if HAS_NIX
        va_list ap;
        va_start(ap, fmt);
        set_color(CYAN_BOLD);
        vprintf(fmt, ap);
        va_end(ap);
        set_color(REWIND);
    #elif HAS_WIN
        va_list ap;
        va_start(ap, fmt);
        set_color(CYAN);
        vprintf(fmt, ap);
        va_end(ap);
        set_color(REWIND);
    #else
        #error Platform not supported by color-sh lib
    #endif
}

void color_rewind(void) {
    set_color(REWIND);
}
