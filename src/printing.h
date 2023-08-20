#ifndef LISPY_PRINTING_H
#define LISPY_PRINTING_H

#include "common.h"

void lval_print(lval_t* val);
void lval_print_expr(lval_t* val, char open, char close);
void lval_println(lval_t* val);
void lval_print_str(const lval_t* val);
char* ltype_name(int type);

#endif
