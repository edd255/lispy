/// @file eq_cmp.h
/// @brief This file contains all builtin logic methods.

#pragma once

#ifndef LISPY_BUILTINS_LOGIC_H
#define LISPY_BUILTINS_LOGIC_H

#include "common.h"

//---- Logical functions -------------------------------------------------------
/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_logic(lenv_t* env, lval_t* args, char* op);

/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_and(lenv_t* env, lval_t* args);

/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_or(lenv_t* env, lval_t* args);

/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_not(lenv_t* env, lval_t* args);

#endif
