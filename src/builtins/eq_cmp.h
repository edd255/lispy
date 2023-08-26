/// @file eq_cmp.h
/// @brief This file contains all builtin equality comparison methods.

#pragma once

#ifndef LISPY_BUILTINS_EQ_CMP_H
#define LISPY_BUILTINS_EQ_CMP_H

#include "common.h"

//---- Equality comparison functions -------------------------------------------
/// @brief The main function of this module which does the actual computation
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @param op The operator to apply
/// @return A Lispy value containing the result of (op args)
lval_t* builtin_cmp(lenv_t* env, lval_t* args, char* op);

/// @brief Compares whether two values are equal
/// @param env The environment in which the arguments reside
/// @param args A list containing two arguments
/// @return A Lispy value containg 1 if they are equal, 0 else
lval_t* builtin_eq(lenv_t* env, lval_t* args);

/// @brief Compares whether two values are unequal
/// @param env The environment in which the arguments reside
/// @param args A list containing two arguments
/// @return A Lispy value containg 1 if they are unequal, 0 else
lval_t* builtin_ne(lenv_t* env, lval_t* args);

#endif
