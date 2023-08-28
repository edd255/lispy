/// @file builtins.h
/// @brief This file bundles access to all builtin methods.

#pragma once

#ifndef LISPY_BUILTINS_H
#define LISPY_BUILTINS_H

#include "builtins/arithmetic.h"
#include "builtins/conditional.h"
#include "builtins/env.h"
#include "builtins/eq_cmp.h"
#include "builtins/helpers.h"
#include "builtins/list.h"
#include "builtins/logic.h"
#include "builtins/magn_cmp.h"
#include "builtins/strings.h"
#include "builtins/vars.h"
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
