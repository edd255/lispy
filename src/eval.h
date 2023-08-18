#ifndef HOME_EDD_CODE_LISPY_SRC_EVAL_H
#define HOME_EDD_CODE_LISPY_SRC_EVAL_H

#include "common.h"

lval_t* lval_eval(lenv_t* e, lval_t* v);
lval_t* lval_eval_sexpr(lenv_t* e, lval_t* v);

#endif
