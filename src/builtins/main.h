/// @file main.h
/// @brief All builtin methods are added to the environment which is loaded at
/// start.

#pragma once

#ifndef LISPY_BUILTINS_MAIN_H
#define LISPY_BUILTINS_MAIN_H

#include "common.h"

/// @brief Adds all builtin methods to the given environment.
/// @param env The environment where the builtins should be added.
void lenv_add_builtins(lenv_t* env);

/// @brief A helper function to add builtin methods to the given environment.
/// @param env The environment where the builtin functions are added.
/// @param name The names given to the builtin methods
/// @param fn Pointers to the builtin methods
void lenv_add_builtin_fn(lenv_t* env, char* name, lbuiltin_t fn);

/// @brief A helper function to add builtin constants to the given environment.
/// @param env The environment where the builtin functions are added.
/// @param name The names given to the builtin methods
/// @param fn Pointers to the builtin constant
void lenv_add_builtin_const(lenv_t* env, char* name, lval_t* value);

#endif
