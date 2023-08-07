#ifndef EVAL_H
#define EVAL_H

#include "common.h"

lval_t* lval_eval(lenv_t* e, lval_t* v);
lval_t* lval_eval_sexpr(lenv_t* e, lval_t* v);

#endif
