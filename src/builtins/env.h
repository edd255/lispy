/// @file env.h
/// @brief This file contains all builtin environment methods.

#pragma once

#ifndef LISPY_BUILTINS_ENV_H
#define LISPY_BUILTINS_ENV_H

#include "common.h"

/// @brief Prints out all values in the environment
///
/// https://github.com/Federico-abss/Lispy/blob/master/lispy.c#L1074
///
/// @param env The environment whose values we want to print
/// @param args Necessary but unused arguments
/// @return A quoted expression that contains the names of all values
lval* builtin_env(lenv* env, lval* args);

/// @brief A builtin-method to define new functions
///
/// https://github.com/Federico-abss/Lispy/blob/master/lispy.c#L1110
///
/// @param env The environment in which the function is put
/// @param args Body, arguments, and name of the function
/// @return An empty symbolic expression, else runtime errors
lval* builtin_fun(lenv* env, lval* args);

#endif
