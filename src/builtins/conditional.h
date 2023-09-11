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
lval* builtin_if(lenv* env, lval* args);

/// @brief Evaluates a condition. On fail, prints expected and actual values.
///
/// Hacky solution: To print expected and actual values, the have to be given to
/// the test function additionally to the condition.
///
/// @param env The environment in which the expression is used
/// @param args The condition, the expected and actual values
/// @return "PASSED" on success, "FAIL" and expected and actual values else
lval* builtin_testhelper(lenv* env, lval* args);

/// @brief Evaluates a series of conditions
/// @param env The environment in which the expression is used
/// @param args A symbolic expression, that contains quoted expression which
/// themselves contain one condition and one body that should be executed if the
/// condition evaluates to true
/// @return The evaluated body of the true condition
lval* builtin_select(lenv* env, lval* args);

/// @brief Evaluates switch-case statements
/// @param env The environment in which the expression is used
/// @param args The first argument is the argument to match, the residuals
/// arguments are quoted expression that contain as first element the case, and
/// as second argument the result
/// @return The evaluated body of the true condition
lval* builtin_case(lenv* env, lval* args);

#endif
