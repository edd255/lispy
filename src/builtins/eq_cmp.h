#ifndef HOME_EDD_CODE_LISPY_SRC_BUILTINS_EQ_CMP_H
#define HOME_EDD_CODE_LISPY_SRC_BUILTINS_EQ_CMP_H

#include "../common.h"

//---- Equality comparison functions -------------------------------------------
lval_t* builtin_cmp(lenv_t* e, lval_t* a, char* op);
lval_t* builtin_eq(lenv_t* e, lval_t* a);
lval_t* builtin_ne(lenv_t* e, lval_t* a);

#endif
