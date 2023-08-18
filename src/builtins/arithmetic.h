#ifndef LISPY_BUILTINS_ARITHMETIC_H
#define LISPY_BUILTINS_ARITHMETIC_H

#include "../common.h"

//---- Arithmetic functions ----------------------------------------------------
lval_t* builtin_op(lenv_t* e, lval_t* a, char* op);
lval_t* builtin_add(lenv_t* e, lval_t* a);
lval_t* builtin_sub(lenv_t* e, lval_t* a);
lval_t* builtin_mul(lenv_t* e, lval_t* a);
lval_t* builtin_div(lenv_t* e, lval_t* a);
lval_t* builtin_mod(lenv_t* e, lval_t* a);
lval_t* builtin_pow(lenv_t* e, lval_t* a);
lval_t* builtin_max(lenv_t* e, lval_t* a);
lval_t* builtin_min(lenv_t* e, lval_t* a);

#endif
