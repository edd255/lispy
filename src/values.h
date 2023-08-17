#ifndef LISPY_VALUES_H
#define LISPY_VALUES_H

#include "common.h"

lval_t* lval_copy(lval_t* v);
lval_t* lval_add(lval_t* v, lval_t* x);
lval_t* lval_join(lval_t* x, lval_t* y);
lval_t* lval_pop(lval_t* v, const int i);
lval_t* lval_take(lval_t* v, const int i);
lval_t* lval_call(lenv_t* e, lval_t* f, lval_t* a);
int lval_eq(const lval_t* x, const lval_t* y);

#endif
