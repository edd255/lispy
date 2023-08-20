#ifndef LISPY_VALUES_H
#define LISPY_VALUES_H

#include "common.h"

lval_t* lval_copy(const lval_t* val);
lval_t* lval_add(lval_t* val, lval_t* x);
lval_t* lval_join(lval_t* x, lval_t* y);
lval_t* lval_pop(lval_t* val, int idx);
lval_t* lval_take(lval_t* val, int idx);
lval_t* lval_call(lenv_t* env, lval_t* fn, lval_t* a);
int lval_eq(const lval_t* x, const lval_t* y);

#endif
