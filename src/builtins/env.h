#ifndef HOME_EDD_CODE_LISPY_SRC_BUILTINS_ENV_H
#define HOME_EDD_CODE_LISPY_SRC_BUILTINS_ENV_H

#include "../common.h"

lval_t* builtin_env(lenv_t* e, lval_t* a);
lval_t* builtin_fun(lenv_t* e, lval_t* a);

#endif
