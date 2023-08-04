#include "../deps/mpc/mpc.h"
#include <editline/readline.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//=== DECLARATIONS =============================================================
//--- Structs ------------------------------------------------------------------
struct lval_t;
struct lenv_t;
typedef struct lval_t lval_t;
typedef struct lenv_t lenv_t;
typedef lval_t*(*lbuiltin_t)(lenv_t*, lval_t*);

//--- Constructors & Destructors -----------------------------------------------
lval_t* lval_num(long x);
lval_t* lval_err(char* fmt, ...);
lval_t* lval_sym(char* s);
lval_t* lval_sexpr(void);
lval_t* lval_qexpr(void);
lval_t* lval_fn(lbuiltin_t fn);
void lval_del(lval_t* v);

//--- Values -------------------------------------------------------------------
lval_t* lval_copy(lval_t* v);
lval_t* lval_add(lval_t* v, lval_t* x);
lval_t* lval_join(lval_t* x, lval_t* y);
lval_t* lval_pop(lval_t* v, int i);
lval_t* lval_take(lval_t* v, int i);

//--- Printing -----------------------------------------------------------------
void lval_print(lval_t* v);
void lval_print_expr(lval_t* v, char open, char close);
void lval_println(lval_t* v);
char* ltype_name(int t);

//--- Environment --------------------------------------------------------------
lenv_t* lenv_new(void);
void lenv_del(lenv_t* e);
lval_t* lenv_get(lenv_t* e, lval_t* k);
void lenv_put(lenv_t* e, lval_t* k, lval_t* v);

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
        args -> count == num, \
        "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
        fn, \
        args -> count, \
        num \
    )

#define LASSERT_NOT_EMPTY(fn, args, index) \
    LASSERT( \
        args, \
        args -> cell[index] -> count != 0, \
        "Function '%s' passed  for argument %i.", \
        fn, \
        index \
    )

#define UNUSED(x) (void) x

//--- Builtins -----------------------------------------------------------------
lval_t* builtin_list(lenv_t* e, lval_t* a);
lval_t* builtin_head(lenv_t* e, lval_t* a);
lval_t* builtin_tail(lenv_t* e, lval_t* a);
lval_t* builtin_eval(lenv_t* e, lval_t* a);
lval_t* builtin_join(lenv_t* e, lval_t* a);
lval_t* builtin_op(lenv_t* e, lval_t* a, char* op);
lval_t* builtin_add(lenv_t* e, lval_t* a);
lval_t* builtin_sub(lenv_t* e, lval_t* a);
lval_t* builtin_mul(lenv_t* e, lval_t* a);
lval_t* builtin_div(lenv_t* e, lval_t* a);
lval_t* builtin_def(lenv_t* e, lval_t* a);
void lenv_add_builtin(lenv_t* e, char* name, lbuiltin_t func);
void lenv_add_builtins(lenv_t* e);

//--- Evaluation ---------------------------------------------------------------
lval_t* lval_eval(lenv_t* e, lval_t* v);
lval_t* lval_eval_sexpr(lenv_t* e, lval_t* v);

//--- Reading ------------------------------------------------------------------
lval_t* lval_read_num(mpc_ast_t* t);
lval_t* lval_read(mpc_ast_t* t);

//--- Main funciton ------------------------------------------------------------
int main(void);


//=== STRUCTS AND ENUMS ========================================================

/* Create enumeration of possible error types */
enum {
    LERR_DIV_ZERO,
    LERR_BAD_OP,
    LERR_BAD_NUM
};

/* Create enumeration of possible lval_t types */
enum {
    LVAL_ERR,
    LVAL_NUM,
    LVAL_SYM,
    LVAL_SEXPR,
    LVAL_QEXPR,
    LVAL_FN
};

struct lval_t {
    int type;
    long num;

    /* Error and Symbol types have some string data */
    char* err;
    char* sym;
    lbuiltin_t fn;

    /* Count and Pointer to a list of lval_t */
    int count;
    lval_t** cell;
};

struct lenv_t {
    int count;
    char** syms;
    lval_t** vals;
};
