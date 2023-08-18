#ifndef HOME_EDD_CODE_LISPY_SRC_BUILTINS_VARS_H
#define HOME_EDD_CODE_LISPY_SRC_BUILTINS_VARS_H

#include "../common.h"

//---- Variable functions ------------------------------------------------------
lval_t* builtin_var(lenv_t* e, lval_t* a, char* fn);
lval_t* builtin_lambda(lenv_t* e, lval_t* a);
lval_t* builtin_def(lenv_t* e, lval_t* a);
lval_t* builtin_put(lenv_t* e, lval_t* a);

#endif
