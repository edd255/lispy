#ifndef LISPY_BUILTINS_MAGN_CMP_H
#define LISPY_BUILTINS_MAGN_CMP_H

#include "../common.h"

//---- Magnitude comparison functions ------------------------------------------
lval_t* builtin_ord(lenv_t* e, lval_t* a, char* op);
lval_t* builtin_gt(lenv_t* e, lval_t* a);
lval_t* builtin_lt(lenv_t* e, lval_t* a);
lval_t* builtin_ge(lenv_t* e, lval_t* a);
lval_t* builtin_le(lenv_t* e, lval_t* a);

#endif