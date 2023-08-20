#ifndef LISPY_BUILTINS_ARITHMETIC_H
#define LISPY_BUILTINS_ARITHMETIC_H

#include "common.h"

//---- Arithmetic functions ----------------------------------------------------
lval_t* builtin_op(lenv_t* env, lval_t* args, char* op);
lval_t* builtin_add(lenv_t* env, lval_t* args);
lval_t* builtin_sub(lenv_t* env, lval_t* args);
lval_t* builtin_mul(lenv_t* env, lval_t* args);
lval_t* builtin_div(lenv_t* env, lval_t* args);
lval_t* builtin_mod(lenv_t* env, lval_t* args);
lval_t* builtin_pow(lenv_t* env, lval_t* args);
lval_t* builtin_max(lenv_t* env, lval_t* args);
lval_t* builtin_min(lenv_t* env, lval_t* args);

#endif
