#ifndef LISPY_READING_H
#define LISPY_READING_H

#include "common.h"

lval_t* lval_read_num(const mpc_ast_t* t);
lval_t* lval_read(mpc_ast_t* t);
lval_t* lval_read_str(mpc_ast_t* tree);

#endif
