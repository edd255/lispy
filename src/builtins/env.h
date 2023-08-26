/// @file env.h
/// @brief This file contains all builtin environment methods.

#pragma once

#ifndef LISPY_BUILTINS_ENV_H
#define LISPY_BUILTINS_ENV_H

#include "common.h"

/// @brief Prints out all values in the environment
/// @param env The environment whose values we want to print
/// @param args Necessary but unused arguments
/// @return A quoted expression that contains the names of all values
lval_t* builtin_env(lenv_t* env, lval_t* args);

/// @brief A builtin-method to define new functions
/// @param env The environment in which the function is put
/// @param args Body, arguments, and name of the function
/// @return An empty symbolic expression, else runtime errors
lval_t* builtin_fun(lenv_t* env, lval_t* args);

#endif
