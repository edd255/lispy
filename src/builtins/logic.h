#ifndef LISPY_BUILTINS_LOGIC_H
#define LISPY_BUILTINS_LOGIC_H

#include "../common.h"

//---- Logical functions -------------------------------------------------------
lval_t* builtin_logic(lenv_t* e, lval_t* a, char* op);
lval_t* builtin_and(lenv_t* e, lval_t* a);
lval_t* builtin_or(lenv_t* e, lval_t* a);
lval_t* builtin_not(lenv_t* e, lval_t* a);

#endif
