#ifndef LISPY_BUILTINS_STRINGS_H
#define LISPY_BUILTINS_STRINGS_H

#include "../common.h"

//---- String functions --------------------------------------------------------
lval_t* builtin_load(lenv_t* e, lval_t* a);
lval_t* builtin_print(lenv_t* e, lval_t* a);
lval_t* builtin_error(lenv_t* e, lval_t* a);

#endif
