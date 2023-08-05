#pragma once

#include "common.h"

//--- Reading ------------------------------------------------------------------
lval_t* lval_read_num(mpc_ast_t* t);
lval_t* lval_read(mpc_ast_t* t);
lval_t* lval_read_str(mpc_ast_t* tree);
