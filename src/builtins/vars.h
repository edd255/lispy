/// @file eq_cmp.h
/// @brief This file contains all builtin variable methods.

#pragma once

#ifndef LISPY_BUILTINS_VARS_H
#define LISPY_BUILTINS_VARS_H

#include "common.h"

//---- Variable functions ------------------------------------------------------
/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_var(lenv_t* env, lval_t* args, char* fn);

/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_lambda(lenv_t* env, lval_t* args);

/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_def(lenv_t* env, lval_t* args);

/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_put(lenv_t* env, lval_t* args);

#endif
