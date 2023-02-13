#ifndef CLISP_EVAL_H
#define CLISP_EVAL_H

#include <errno.h>
#include "../libs/mpc/mpc.h"
#include "lisp_value.h"
#include "lisp_value.h"


lisp_value_t eval_op(lisp_value_t x, char* op, lisp_value_t y);

lisp_value_t eval(mpc_ast_t* ast);

#endif
