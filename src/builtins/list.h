/// @file list.h
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
lval* builtin_list(lenv* env, lval* args);

/// @brief Computes the head of a list or string
/// @param env The environment in which the arguments are used
/// @param args The list/string
/// @return Head of the list/string
lval* builtin_head(lenv* env, lval* args);

/// @brief Computes the tail of a list or string
/// @param env The environment in which the arguments are used
/// @param args The list/string
/// @return Tail of the list/string
lval* builtin_tail(lenv* env, lval* args);

/// @brief Evaluates a quoted expression
///
/// Expects that the first argument of the quoted expression is a function, and
/// the residuals are arguments to this function.
///
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The result of the evaluation
lval* builtin_eval(lenv* env, lval* args);

/// @brief Joins two quoted expressions/two strings
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return A quoted expression/string that represents the join
lval* builtin_join(lenv* env, lval* args);

/// @brief Appends a value to the front of a quoted expression
///
/// https://github.com/Federico-abss/Lispy/blob/master/lispy.c#L960
///
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The resulting quoted expression
lval* builtin_cons(lenv* env, lval* args);

/// @brief Computes the length of the quoted expression/string
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The length of the quoted expression/string
lval* builtin_len(lenv* env, lval* args);

/// @brief Applies a function to a variable number of arguments.
///
/// Expects that the first argument is a function.
///
/// https://github.com/Federico-abss/Lispy/blob/master/lispy.c#L1038
///
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The result of the evaluation.
lval* builtin_pack(lenv* env, lval* args);

/// @brief Applies a function to a list of arguments.
///
/// Expects that the first argument is a function.
///
/// https://github.com/Federico-abss/Lispy/blob/master/lispy.c#L1055
///
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The result of the evaluation.
lval* builtin_unpack(lenv* env, lval* args);

/// @brief Returns the nth element of a list or string.
///
/// Expects that the first argument is a number.
///
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The nth element of the given list/string.
lval* builtin_nth(lenv* env, lval* args);

/// @brief Returns the first element of a list or string.
///
/// Expects that the first argument is a number.
///
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The first element of the given list/string.
lval* builtin_first(lenv* env, lval* args);

/// @brief Returns the last element of a list or string.
///
/// Expects that the first argument is a number.
///
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The second element of the given list/string.
lval* builtin_second(lenv* env, lval* args);

/// @brief Returns the last element of a list or string.
///
/// Expects that the first argument is a number.
///
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The third element of the given list/string.
lval* builtin_third(lenv* env, lval* args);

/// @brief Returns the last element of a list or string.
///
/// Expects that the first argument is a number.
///
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return The last element of the given list/string.
lval* builtin_last(lenv* env, lval* args);

/// @brief Checks whether the first argument is element in the list/string
/// @param env The environment in which the arguments are used
/// @param args The element to check and the list/string
/// @return 1 if the argument is in the list/string, else 0
lval* builtin_elem(lenv* env, lval* args);

/// @brief Returns all of list/string but last element
/// @param env The environment in which the arguments are used
/// @param args The list/string
/// @return The list/string but w/o last element
lval* builtin_init(lenv* env, lval* args);

/// @brief Takes n items from list/string
/// @param env The environment in which the arguments are used
/// @param args The list/string
/// @return A list/string with the first n items of the given list/string
lval* builtin_take(lenv* env, lval* args);

/// @brief Drops n items from list/string
/// @param env The environment in which the arguments are used
/// @param args The list/string
/// @return A list/string without the first n items of the given list/string
lval* builtin_drop(lenv* env, lval* args);

/// @brief Splits a list/string at n
/// @param env The environment in which the arguments are used
/// @param args The list/string
/// @return A qexpr with two elements: The first contains the first n elements of
/// the given list/string, the second contains the rest
lval* builtin_split(lenv* env, lval* args);

#endif
