#pragma once

#ifndef LISPY_IO_H
#define LISPY_IO_H

#include "common.h"
#include "deps/mpc/mpc.h"

//=== WRITING ==================================================================
void lval_print(lval_t* val);
void lval_print_expr(lval_t* val, char open, char close);
void lval_println(lval_t* val);
void lval_print_str(const lval_t* val);
char* ltype_name(int type);

//=== READING ==================================================================
lval_t* lval_read_num(const mpc_ast_t* tree);
lval_t* lval_read(mpc_ast_t* tree);
lval_t* lval_read_str(mpc_ast_t* tree);

#endif
