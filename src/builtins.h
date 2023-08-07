#ifndef BUILTINS_H
#define BUILTINS_H

#include "common.h"

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
lval_t* builtin_mod(lenv_t* e, lval_t* a);
lval_t* builtin_var(lenv_t* e, lval_t* a, char* fn);
lval_t* builtin_def(lenv_t* e, lval_t* a);
lval_t* builtin_put(lenv_t* e, lval_t* a);
lval_t* builtin_lambda(lenv_t* e, lval_t* a);
lval_t* builtin_gt(lenv_t* e, lval_t* a);
lval_t* builtin_lt(lenv_t* e, lval_t* a);
lval_t* builtin_ge(lenv_t* e, lval_t* a);
lval_t* builtin_le(lenv_t* e, lval_t* a);
lval_t* builtin_ord(lenv_t* e, lval_t* a, char* op);
lval_t* builtin_cmp(lenv_t* e, lval_t* a, char* op);
lval_t* builtin_eq(lenv_t* e, lval_t* a);
lval_t* builtin_ne(lenv_t* e, lval_t* a);
lval_t* builtin_if(lenv_t* e, lval_t* a);
lval_t* builtin_load(lenv_t* e, lval_t* a);
lval_t* builtin_print(lenv_t* e, lval_t* a);
lval_t* builtin_error(lenv_t* e, lval_t* a);
void lenv_add_builtin(lenv_t* e, char* name, lbuiltin_t func);
void lenv_add_builtins(lenv_t* e);

#endif
