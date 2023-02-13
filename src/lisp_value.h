#ifndef LISP_VALUE_H
#define LISP_VALUE_H

#include <stdio.h>

enum { 
        LISP_VAL_NUM,
        LISP_VAL_ERR
};

enum {
        ERR_DIV_ZERO,
        ERR_BAD_OP,
        ERR_BAD_NUM
};

typedef struct {
        int type;
        long number;
        int error;
} lisp_value_t;

/* Create a new number type lval */
lisp_value_t lisp_val_num(long x);

/* Create a new error type lval */
lisp_value_t lisp_val_err(int x);

/* Print a value */
void lisp_val_print(lisp_value_t value);

/* Print an value followed by a newline */
void lisp_val_println(lisp_value_t value);

#endif
