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

/// @brief Evaluates a condition. On fail, prints expected and actual values.
///
/// Hacky solution: To print expected and actual values, the have to be given to
/// the test function additionally to the condition.
///
/// @param env The environment in which the expression is used
/// @param args The condition, the expected and actual values
/// @return "PASSED" on success, "FAIL" and expected and actual values else
lval_t* builtin_testhelper(lenv_t* env, lval_t* args);

lval_t* builtin_select(lenv_t* env, lval_t* args);

#endif
