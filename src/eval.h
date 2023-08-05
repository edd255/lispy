#pragma once

#include "common.h"

//--- Evaluation ---------------------------------------------------------------
lval_t* lval_eval(lenv_t* e, lval_t* v);
lval_t* lval_eval_sexpr(lenv_t* e, lval_t* v);
