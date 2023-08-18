#ifndef LISPY_EVAL_H
#define LISPY_EVAL_H

#include "common.h"

lval_t* lval_eval(lenv_t* e, lval_t* v);
lval_t* lval_eval_sexpr(lenv_t* e, lval_t* v);

#endif
