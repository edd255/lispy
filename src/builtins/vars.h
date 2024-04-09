/// @file vars.h
/// @brief This file contains all builtin variable methods.

#pragma once

#ifndef LISPY_BUILTINS_VARS_H
#define LISPY_BUILTINS_VARS_H

#include "common.h"

//---- Variable functions ------------------------------------------------------
/// @brief Defines a function in the local or global environment
/// @param env The environment to use
/// @param args The function to define
/// @param fn "def" or "="
/// @return An empty symbolic expression or an error
Value* builtin_var(Environment* env, Value* args, char* fn);

/// @brief Define a lambda function
/// @param env The used environment
/// @param args The formal arguments and the body of the lambda function
/// @return The specified lambda function
Value* builtin_lambda(Environment* env, Value* args);

/// @brief Defines a key-value pair globally.
///
/// This method calls builtin_var, and then env_def internally.
///
/// @param env The current environment
/// @param args The function to define
/// @return An empty symbolic expression or an error
Value* builtin_def(Environment* env, Value* args);

/// @brief Define a key-value in an environment
/// @param env The environment in which we define key and value.
/// @param args The function to define
/// @return An empty symbolic expression or an error
Value* builtin_put(Environment* env, Value* args);

#endif
