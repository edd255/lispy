#ifndef LISPY_BUILTINS_MAGN_CMP_H
#define LISPY_BUILTINS_MAGN_CMP_H

#include "../common.h"

//---- Magnitude comparison functions ------------------------------------------
lval_t* builtin_ord(lenv_t* env, lval_t* a, char* operator);
lval_t* builtin_gt(lenv_t* env, lval_t* a);
lval_t* builtin_lt(lenv_t* env, lval_t* a);
lval_t* builtin_ge(lenv_t* env, lval_t* a);
lval_t* builtin_le(lenv_t* env, lval_t* a);

#endif
