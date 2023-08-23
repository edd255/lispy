#pragma once

#ifndef LISPY_BUILTINS_EQ_CMP_H
#define LISPY_BUILTINS_EQ_CMP_H

#include "common.h"

//---- Equality comparison functions -------------------------------------------
lval_t* builtin_cmp(lenv_t* env, lval_t* args, char* op);
lval_t* builtin_eq(lenv_t* env, lval_t* args);
lval_t* builtin_ne(lenv_t* env, lval_t* args);

#endif
