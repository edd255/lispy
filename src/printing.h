#pragma once

#include "common.h"

//--- Printing -----------------------------------------------------------------
void lval_print(lval_t* v);
void lval_print_expr(lval_t* v, char open, char close);
void lval_println(lval_t* v);
void lval_print_str(lval_t* v);
char* ltype_name(int t);
