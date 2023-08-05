#pragma once

#include "common.h"

//--- Values -------------------------------------------------------------------
lval_t* lval_copy(lval_t* v);
lval_t* lval_add(lval_t* v, lval_t* x);
lval_t* lval_join(lval_t* x, lval_t* y);
lval_t* lval_pop(lval_t* v, int i);
lval_t* lval_take(lval_t* v, int i);
lval_t* lval_call(lenv_t* e, lval_t* f, lval_t* a);
int lval_eq(lval_t* x, lval_t* y);
