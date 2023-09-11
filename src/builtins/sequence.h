/// @file strings.h
/// @brief This file contains all builtin sequential execution methods.

#pragma once

#ifndef LISPY_BUILTINS_SEQUENCE_H
#define LISPY_BUILTINS_SEQUENCE_H

#include "common.h"

/// @brief Evaluates all given symbolic expressions in sequence
/// @param env The used environment
/// @param args All symbolic expressions to execute
/// @return An empty symbolic expression
lval_t* builtin_do(lenv_t* env, lval_t* args);

#endif
