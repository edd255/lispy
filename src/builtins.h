/// @file builtins.h
/// @brief This file bundles access to all builtin methods.

#pragma once

#ifndef LISPY_BUILTINS_H
#define LISPY_BUILTINS_H

#include "common.h"

/// @brief Adds all builtin methods to the given environment.
/// @param env The environment where the builtins should be added.
void env_add_builtins(Environment* env);

/// @brief A helper function to add builtin methods to the given environment.
/// @param env The environment where the builtin functions are added.
/// @param name The names given to the builtin methods
/// @param fn Pointers to the builtin methods
void env_add_builtin_fn(Environment* env, char* name, Function fn);

/// @brief A helper function to add builtin constants to the given environment.
/// @param env The environment where the builtin functions are added.
/// @param name The names given to the builtin methods
/// @param value Pointers to the builtin constant
void env_add_builtin_const(Environment* env, char* name, Value* value);

#endif
