/// @file misc.h
/// @brief This file contains all builtin miscellaneous methods.

#pragma once

#ifndef LISPY_BUILTINS_MISC_H
#define LISPY_BUILTINS_MISC_H

#include "common.h"

//---- Miscellaneous functions -------------------------------------------------
/// @brief Takes a function, two arguments, and flips the arguments
/// @param env The environment to use
/// @param args The arguments
/// @return The evaluated function
Value* builtin_flip(Environment* env, Value* args);

/// @brief Takes two functions, one argument, and calculcates f (g x)
/// @param env The used environment
/// @param args The arguments
/// @return The evaluated function
Value* builtin_comp(Environment* env, Value* args);

#endif
