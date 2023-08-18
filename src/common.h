#ifndef HOME_EDD_CODE_LISPY_SRC_COMMON_H
#define HOME_EDD_CODE_LISPY_SRC_COMMON_H

#include <stdbool.h>

#include "../deps/logger/log.h"
#include "../deps/mpc/mpc.h"

//=== DECLARATIONS =============================================================
//--- Macros -------------------------------------------------------------------
#define LASSERT(args, cond, fmt, ...) \
    if (!(cond)) { \
        lval_t* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(args); \
        return err; \
    }

#define LASSERTS(args, cond1, cond2, fmt, ...) \
    if (!(cond1 || cond2)) { \
        lval_t* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(args); \
        return err; \
    }

#define LASSERT_TYPE(fn, args, idx, expect) \
    LASSERT( \
        args, \
        (args)->cell[idx]->type == (expect), \
        "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
        fn, \
        idx, \
        ltype_name((args)->cell[idx]->type), \
        ltype_name(expect) \
    )

#define LASSERT_TYPES(fn, args, idx, expect1, expect2) \
    LASSERTS( \
        args, \
        (args)->cell[idx]->type == (expect1), \
        (args)->cell[idx]->type == (expect2), \
        "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s or %s.", \
        fn, \
        idx, \
        ltype_name((args)->cell[idx]->type), \
        ltype_name(expect1), \
        ltype_name(expect2) \
    )

#define LASSERT_NUM(fn, args, num) \
    LASSERT( \
        args, \
        (args)->count == (num), \
        "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
        fn, \
        (args)->count, \
        num \
    )

#define LASSERT_NOT_EMPTY(fn, args, index) \
    LASSERT( \
        args, \
        (args)->cell[index]->count != 0, \
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
lval_t* lval_dec(double x);
lval_t* lval_err(char* fmt, ...);
lval_t* lval_sym(char* s);
lval_t* lval_sexpr(void);
lval_t* lval_qexpr(void);
lval_t* lval_fn(lbuiltin_t fn);
lval_t* lval_lambda(lval_t* formals, lval_t* body);
lval_t* lval_str(const char* s);
void lval_del(lval_t* v);

//--- Environment --------------------------------------------------------------
lenv_t* lenv_new(void);
void lenv_del(lenv_t* e);
lval_t* lenv_get(lenv_t* e, lval_t* k);
void lenv_put(lenv_t* e, const lval_t* k, lval_t* v);
lenv_t* lenv_copy(lenv_t* e);
void lenv_def(lenv_t* e, const lval_t* k, lval_t* v);

//=== STRUCTS AND ENUMS ========================================================

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
} __attribute__((packed)) __attribute__((aligned(128)));

struct lenv_t {
    lenv_t* parent;
    int count;
    char** syms;
    lval_t** vals;
} __attribute__((aligned(32)));

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
    #define LOG_MALLOC(size) log_malloc(size, __func__, __FILE__, __LINE__)
    #define LOG_REALLOC(old_ptr, size) \
        log_realloc(old_ptr, size, __func__, __FILE__, __LINE__)
    #define LOG_FREE(ptr) log_free(ptr, __func__, __FILE__, __LINE__)
#else
    #define LOG_MALLOC(size)           malloc(size)
    #define LOG_REALLOC(old_ptr, size) realloc(old_ptr, size)
    #define LOG_FREE(ptr)              free(ptr)
#endif

#endif
