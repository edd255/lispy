#ifndef LISPY_BUILTINS_ENV_H
#define LISPY_BUILTINS_ENV_H

#include "../common.h"

lval_t* builtin_env(lenv_t* e, lval_t* a);
lval_t* builtin_fun(lenv_t* e, lval_t* a);

#endif
