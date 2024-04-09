/// @file magn_cmp.h
/// @brief This file contains all builtin magnitude comparison methods.

#pragma once

#ifndef LISPY_BUILTINS_MAGN_CMP_H
#define LISPY_BUILTINS_MAGN_CMP_H

#include "common.h"

//---- Magnitude comparison functions ------------------------------------------
/// @brief The main function of this module which does the actual computation
/// @param env The environment in which the arguments are used
/// @param args The arguments to use
/// @param op The operator to apply
/// @return The result of (op args)
Value* builtin_ord(Environment* env, Value* args, char* op);

/// @brief Compares two argument using greater-than
/// @param env The environment to use
/// @param args The arguments to compare
/// @return Returns a if a >= b else b
Value* builtin_gt(Environment* env, Value* args);

/// @brief
/// @param env The environment to use
/// @param args The arguments to compare
/// @return Returns a if a <= b else b
Value* builtin_lt(Environment* env, Value* args);

/// @brief
/// @param env The environment to use
/// @param args The arguments to compare
/// @return Returns a if a > b else b
Value* builtin_ge(Environment* env, Value* args);

/// @brief
/// @param env The environment to use
/// @param args The arguments to compare
/// @return Returns a if a < b else b
Value* builtin_le(Environment* env, Value* args);

#endif
