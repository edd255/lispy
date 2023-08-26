/// @file eq_cmp.h
/// @brief This file contains all builtin string methods.

#pragma once

#ifndef LISPY_BUILTINS_STRINGS_H
#define LISPY_BUILTINS_STRINGS_H

#include "common.h"

//---- String functions --------------------------------------------------------
/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_load(lenv_t* env, lval_t* args);

/// @brief Prints a string
/// @param env The used environment
/// @param args The arguments to print
/// @return An empty symbolic expression
lval_t* builtin_print(lenv_t* env, lval_t* args);

/// @brief Constructs an error from the first argument
/// @param env The used environment
/// @param args The error to use
/// @return An Lispy value that represents the error
lval_t* builtin_error(lenv_t* env, lval_t* args);

#endif
