#ifndef CLISP_EVAL_H
#define CLISP_EVAL_H

#include "../libs/mpc/mpc.h"

long eval_op(long x, char* op, long y);

long eval(mpc_ast_t* ast);

#endif
