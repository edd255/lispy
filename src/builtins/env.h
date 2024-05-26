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
Value* builtin_env(Environment* env, Value* args);

/// @brief A builtin-method to define new functions
///
/// https://github.com/Federico-abss/Lispy/blob/master/lispy.c#L1110
///
/// @param[out] env The environment in which the function is put
/// @param args Body, arguments, and name of the function
/// @return An empty symbolic expression, else runtime errors
Value* builtin_fun(Environment* env, Value* args);

/// @brief Loads the content of a file when given the name of the file
/// @param[out] env The environment to load the content in
/// @param args The file name
/// @return An empty list or an error if it fails
Value* builtin_load(Environment* env, Value* args);

#endif
