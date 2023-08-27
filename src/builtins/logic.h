/// @file logic.h
/// @brief This file contains all builtin logic methods.

#pragma once

#ifndef LISPY_BUILTINS_LOGIC_H
#define LISPY_BUILTINS_LOGIC_H

#include "common.h"

//---- Logical functions -------------------------------------------------------
/// @brief The main function of this module which does the actual computation
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @param op The operator to apply
/// @return The result of (op args)
lval_t* builtin_logic(lenv_t* env, lval_t* args, char* op);

/// @brief Computes the and of two arguments
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return A Lispy value which contains the and of all arguments
lval_t* builtin_and(lenv_t* env, lval_t* args);

/// @brief Computes the or of two arguments
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @return A Lispy value which contains the or of all arguments
lval_t* builtin_or(lenv_t* env, lval_t* args);

/// @brief Computes the not of one arguments
/// @param env The environment in which the argument is used
/// @param args The argument to use
/// @return A Lispy value which contains the not of all arguments
lval_t* builtin_not(lenv_t* env, lval_t* args);

#endif
