/// @file arithmetic.h
/// @brief This file contains all builtin arithmetic methods.

#pragma once

#ifndef LISPY_BUILTINS_ARITHMETIC_H
#define LISPY_BUILTINS_ARITHMETIC_H

#include "common.h"

//---- Arithmetic functions ----------------------------------------------------
/// @brief The main function of this module which does the actual computation
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @param op The operator to apply
/// @return The result of (op args)
lval* builtin_op(lenv* env, lval* args, char* op);

/// @brief Adds all arguments together
/// @param env The environment in which the arguments are added
/// @param args The arguments to add
/// @return A Lispy value which contains the sum of all arguments
lval* builtin_add(lenv* env, lval* args);

/// @brief Subtracts all arguments
/// @param env The environment in which the arguments are subtracted
/// @param args The arguments to subtract
/// @return A Lispy value which contains the difference of all arguments
lval* builtin_sub(lenv* env, lval* args);

/// @brief Multiplies all arguments together
/// @param env The environment in which the arguments are multiplied
/// @param args The arguments to multiply
/// @return A Lispy value which contains the product of all arguments
lval* builtin_mul(lenv* env, lval* args);

/// @brief Divides all arguments
/// @param env The environment in which the arguments are divided
/// @param args The arguments to divide
/// @return A Lispy value which contains the quotient of all arguments
lval* builtin_div(lenv* env, lval* args);

/// @brief Applies modular division to all arguments
/// @param env The environment in which the arguments are used
/// @param args The arguments to use for modular division
/// @return A Lispy value which contains the return of the modular division of all arguments
lval* builtin_mod(lenv* env, lval* args);

/// @brief Exponentiates all arguments
/// @param env The environment in which the arguments are exponentiated
/// @param args The arguments to exponentiate
/// @return A Lispy value which contains the power of all arguments
lval* builtin_pow(lenv* env, lval* args);

/// @brief Finds the maximum of all arguments
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return A Lispy value which contains the maximum of all arguments
lval* builtin_max(lenv* env, lval* args);

/// @brief Finds the minimum of all arguments
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return A Lispy value which contains the minimum of all arguments
lval* builtin_min(lenv* env, lval* args);

/// @brief Computes the sum of all arguments in the q-expr
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return A Lispy value which contains the sum of all arguments
lval* builtin_sum(lenv* env, lval* args);

/// @brief Computes the product of all arguments in the q-expr
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return A Lispy value which contains the product of all arguments
lval* builtin_prod(lenv* env, lval* args);

#endif
