/// @file conditional.h
/// @brief This file contains all builtin conditional methods.

#pragma once

#ifndef LISPY_BUILTINS_CONDITIONAL_H
#define LISPY_BUILTINS_CONDITIONAL_H

#include "common.h"

//---- Conditional functions ---------------------------------------------------
/// @brief Evaluates if-expressions
/// @param env The environment in which the expression is used
/// @param args The condition, and the consequences
/// @return The evaluation of the consequence
lval_t* builtin_if(lenv_t* env, lval_t* args);

#endif
