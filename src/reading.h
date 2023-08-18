#ifndef HOME_EDD_CODE_LISPY_SRC_READING_H
#define HOME_EDD_CODE_LISPY_SRC_READING_H

#include "common.h"

lval_t* lval_read_num(const mpc_ast_t* t);
lval_t* lval_read(mpc_ast_t* t);
lval_t* lval_read_str(mpc_ast_t* tree);

#endif
