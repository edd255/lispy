/// @file eq_cmp.h
/// @brief This file contains all builtin list methods.

#pragma once

#ifndef LISPY_BUILTINS_LIST_H
#define LISPY_BUILTINS_LIST_H

#include "common.h"

//---- List functions ----------------------------------------------------------
/// @brief Computes a quoted expression containing the arguments
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return Returns a quoted expression which contains the arguments
lval_t* builtin_list(lenv_t* env, lval_t* args);

/// @brief Computes the head of a list or string
/// @param env The environment in which the arguments are used
/// @param args The list/string
/// @return Head of the list/string
lval_t* builtin_head(lenv_t* env, lval_t* args);

/// @brief Computes the tail of a list or string
/// @param env The environment in which the arguments are used
/// @param args The list/string
/// @return Tail of the list/string
lval_t* builtin_tail(lenv_t* env, lval_t* args);

/// @brief Evaluates a quoted expression
///
/// Expects that the first argument of the quoted expression is a function, and
/// the residuals are arguments to this function.
///
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The result of the evaluation
lval_t* builtin_eval(lenv_t* env, lval_t* args);

/// @brief Joins two quoted expressions/two strings
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return A quoted expression/string that represents the join
lval_t* builtin_join(lenv_t* env, lval_t* args);

/// @brief Appends a value to the front of a quoted expression
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The resulting quoted expression
lval_t* builtin_cons(lenv_t* env, lval_t* args);

/// @brief Computes the length of the quoted expression/string
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The length of the quoted expression/string
lval_t* builtin_len(lenv_t* env, lval_t* args);

/// @brief Applies a function to a variable number of arguments.
///
/// Expects that the first argument is a function.
///
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The result of the evaluation.
lval_t* builtin_pack(lenv_t* env, lval_t* args);

/// @brief Applies a function to a list of arguments.
///
/// Expects that the first argument is a function.
///
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The result of the evaluation.
lval_t* builtin_unpack(lenv_t* env, lval_t* args);

#endif
