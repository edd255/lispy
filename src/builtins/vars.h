#ifndef LISPY_BUILTINS_VARS_H
#define LISPY_BUILTINS_VARS_H

#include "../common.h"

//---- Variable functions ------------------------------------------------------
lval_t* builtin_var(lenv_t* env, lval_t* a, char* fn);
lval_t* builtin_lambda(lenv_t* env, lval_t* a);
lval_t* builtin_def(lenv_t* env, lval_t* a);
lval_t* builtin_put(lenv_t* env, lval_t* a);

#endif
