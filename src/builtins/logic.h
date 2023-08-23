#pragma once

#ifndef LISPY_BUILTINS_LOGIC_H
#define LISPY_BUILTINS_LOGIC_H

#include "common.h"

//---- Logical functions -------------------------------------------------------
lval_t* builtin_logic(lenv_t* env, lval_t* args, char* op);
lval_t* builtin_and(lenv_t* env, lval_t* args);
lval_t* builtin_or(lenv_t* env, lval_t* args);
lval_t* builtin_not(lenv_t* env, lval_t* args);

#endif
