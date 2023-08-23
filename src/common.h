#pragma once

#ifndef LISPY_COMMON_H
#define LISPY_COMMON_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/alloc.h"
#include "utils/errors.h"

//=== DECLARATIONS =============================================================
//--- Structs ------------------------------------------------------------------
/* Value Struct */
struct lval_t;
typedef struct lval_t lval_t;

/* Environment Struct */
struct lenv_t;
typedef struct lenv_t lenv_t;

/* Builtin Struct */
typedef lval_t* (*lbuiltin_t)(lenv_t*, lval_t*);

//--- Constructors & Destructors for Values ------------------------------------
lval_t* lval_num(long x);
lval_t* lval_dec(double x);
lval_t* lval_err(char* fmt, ...);
lval_t* lval_sym(char* str);
lval_t* lval_sexpr(void);
lval_t* lval_qexpr(void);
lval_t* lval_fn(lbuiltin_t fn);
lval_t* lval_lambda(lval_t* formals, lval_t* body);
lval_t* lval_str(const char* str);

//--- Methods for Values -------------------------------------------------------
lval_t* lval_copy(const lval_t* val);
lval_t* lval_add(lval_t* val, lval_t* x);
lval_t* lval_join(lval_t* x, lval_t* y);
lval_t* lval_pop(lval_t* val, int idx);
lval_t* lval_take(lval_t* val, int idx);
lval_t* lval_call(lenv_t* env, lval_t* fn, lval_t* a);
int lval_eq(const lval_t* x, const lval_t* y);
void lval_del(lval_t* val);

//--- Constructors & Destructors for Environments ------------------------------
lenv_t* lenv_new(void);
void lenv_del(lenv_t* env);

//--- Methods for Environments -------------------------------------------------
lval_t* lenv_get(lenv_t* env, lval_t* key);
void lenv_put(lenv_t* env, const lval_t* key, const lval_t* val);
lenv_t* lenv_copy(lenv_t* env);
void lenv_def(lenv_t* env, const lval_t* key, const lval_t* val);

//=== STRUCTS ==================================================================
struct lval_t {
    // Type
    int type;

    // Basic
    long num;
    double dec;
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

//=== ENUMS ====================================================================
/* Create enumeration of possible error types */
enum LERR { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Create enumeration of possible lval_t types */
enum LVAL {
    LVAL_ERR,
    LVAL_NUM,
    LVAL_DEC,
    LVAL_SYM,
    LVAL_STR,
    LVAL_SEXPR,
    LVAL_QEXPR,
    LVAL_FN
};


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
