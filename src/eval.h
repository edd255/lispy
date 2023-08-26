/// @file eval.h
///
/// @brief This file contains the methods which evaluate values in environments.

#pragma once

#ifndef LISPY_EVAL_H
#define LISPY_EVAL_H

#include "common.h"

/// @brief Evaluates a value in an environment.
///
/// For symbols, we return the associated value in the given environment. For
/// symbolic expression, we call a special function.
///
/// @param env The environment in which to evaluate the given Lispy value
/// @param val The Lispy value to evaluate
/// @return The evaluated Lispy value
lval_t* lval_eval(lenv_t* env, lval_t* val);

/// @brief Evaluates a symbolic expression in an environment.
///
/// For symbolic expressions, we evaluate all the children of the expression. If
/// any of these are errors, we return the first error we encounter. If there
/// are no children, we just return it directly as it's empty. In case only one
/// child exists, we evaluate only the single child.
///
/// Else, we separate the first element of the expression. This element should
/// be a symbol. If this is the case, we pass the symbol and the arguments to
/// builtin_op which does the needed calculations. Else, we return an error.
///
/// @param env The environment in which to evaluate the given Lispy value
/// @param val The Lispy value to evaluate
/// @return The evaluated Lispy value
lval_t* lval_eval_sexpr(lenv_t* env, lval_t* val);

#endif
