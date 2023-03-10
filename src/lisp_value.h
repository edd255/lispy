#ifndef LISP_VALUE_H
#define LISP_VALUE_H

#include <stdio.h>
#include "../deps/mpc/mpc.h"

typedef enum { 
        LISP_VALUE_NUMBER,
        LISP_VALUE_ERROR,
        LISP_VALUE_SYMBOL,
        LISP_VALUE_SEXPR
} LISP_TYPE;

typedef enum {
        LISP_ERROR_DIV_ZERO,
        LISP_ERROR_BAD_OP,
        LISP_ERROR_BAD_NUM
} LISP_ERROR;

typedef struct lisp_value_t lisp_value_t;
struct lisp_value_t {
        LISP_TYPE type;
        long number;
        char* error;
        char* symbol;
        int count;
        lisp_value_t** cell;
};

typedef struct {
        /* Create a pointer to a new number type lval */
        lisp_value_t* (* const number)(long number);

        /* Create a pointer to a new error type lval */
        lisp_value_t* (* const error)(char* message);

        /* Construct a pointer to a new symbol lval */
        lisp_value_t* (* const symbol)(char* symbol);

        /* Construct a pointer to a new empty sexpr lval */
        lisp_value_t* (* const sexpr)(void);

        /* Print a value */
        void (* const print)(lisp_value_t* value);

        /* Print an value followed by a newline */
        void (* const println)(lisp_value_t* value);

        /* Free memory of lisp_value_t objects */
        void (* const free)(lisp_value_t* value);

        /* Read a valuefrom a tree */
        lisp_value_t* (* const read_num)(mpc_ast_t* tree);

        /* Read a valuefrom a tree */
        lisp_value_t* (* const read)(mpc_ast_t* tree);

        /* Add an element to an S-expression */
        lisp_value_t* (* const add)(lisp_value_t* value, lisp_value_t* x);

        /* Extract a single element from an S-Expression at index i */
        lisp_value_t* (* const pop)(lisp_value_t* value, int i);

        /* Deletes the list it has extracted the element at index i from */
        lisp_value_t* (* const take)(lisp_value_t* value, int i);
} namespace_lisp_value;

extern namespace_lisp_value const lisp_value;

#endif
