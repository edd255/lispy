#ifndef LISPY_EVAL_H
#define LISPY_EVAL_H

#include "common.h"

lval_t* lval_eval(lenv_t* env, lval_t* val);
lval_t* lval_eval_sexpr(lenv_t* env, lval_t* val);

#endif
