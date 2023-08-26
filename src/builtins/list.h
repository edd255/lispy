/// @file eq_cmp.h
/// @brief This file contains all builtin list methods.

#pragma once

#ifndef LISPY_BUILTINS_LIST_H
#define LISPY_BUILTINS_LIST_H

#include "common.h"

//---- List functions ----------------------------------------------------------
/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_list(lenv_t* env, lval_t* args);

/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_head(lenv_t* env, lval_t* args);

/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_tail(lenv_t* env, lval_t* args);

/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_eval(lenv_t* env, lval_t* args);

/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_join(lenv_t* env, lval_t* args);

/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_cons(lenv_t* env, lval_t* args);

/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_len(lenv_t* env, lval_t* args);

/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_pack(lenv_t* env, lval_t* args);

/// @brief
/// @param env
/// @param args
/// @return
lval_t* builtin_unpack(lenv_t* env, lval_t* args);

#endif
