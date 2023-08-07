#ifndef COMMON_H
#define COMMON_H

#include <editline/readline.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../deps/mpc/mpc.h"

//=== DECLARATIONS =============================================================
//--- Macros -------------------------------------------------------------------
#define LASSERT(args, cond, fmt, ...) \
    if (!(cond)) { \
        lval_t* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(args); \
        return err; \
    }

#define LASSERT_TYPE(fn, args, index, expect) \
    LASSERT( \
        args, \
        args->cell[index]->type == expect, \
        "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
        fn, \
        index, \
        ltype_name(args->cell[index]->type), \
        ltype_name(expect) \
    )

#define LASSERT_NUM(fn, args, num) \
    LASSERT( \
        args, \
        args->count == num, \
        "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
        fn, \
        args->count, \
        num \
    )

#define LASSERT_NOT_EMPTY(fn, args, index) \
    LASSERT( \
        args, \
        args->cell[index]->count != 0, \
        "Function '%s' passed  for argument %i.", \
        fn, \
        index \
    )

#define UNUSED(x) (void)x

//--- Structs ------------------------------------------------------------------
struct lval_t;
struct lenv_t;
typedef struct lval_t lval_t;
typedef struct lenv_t lenv_t;
typedef lval_t* (*lbuiltin_t)(lenv_t*, lval_t*);

//--- Constructors & Destructors -----------------------------------------------
lval_t* lval_num(long x);
lval_t* lval_err(char* fmt, ...);
lval_t* lval_sym(char* s);
lval_t* lval_sexpr(void);
lval_t* lval_qexpr(void);
lval_t* lval_fn(lbuiltin_t fn);
lval_t* lval_lambda(lval_t* formals, lval_t* body);
lval_t* lval_str(char* s);
void lval_del(lval_t* v);

//--- Environment --------------------------------------------------------------
lenv_t* lenv_new(void);
void lenv_del(lenv_t* e);
lval_t* lenv_get(lenv_t* e, lval_t* k);
void lenv_put(lenv_t* e, lval_t* k, lval_t* v);
lenv_t* lenv_copy(lenv_t* e);
void lenv_def(lenv_t* e, lval_t* k, lval_t* v);

//=== STRUCTS AND ENUMS ========================================================

/* Create enumeration of possible error types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Create enumeration of possible lval_t types */
enum {
    LVAL_ERR,
    LVAL_NUM,
    LVAL_SYM,
    LVAL_STR,
    LVAL_SEXPR,
    LVAL_QEXPR,
    LVAL_FN
};

struct lval_t {
    // Type
    int type;

    // Basic
    long num;
    char* err;
    char* sym;
    char* str;

    // Function
    lbuiltin_t builtin;
    lenv_t* env;
    lval_t* formals;
    lval_t* body;

    // Expression
    int count;
    lval_t** cell;
};

struct lenv_t {
    lenv_t* parent;
    int count;
    char** syms;
    lval_t** vals;
};

#endif
