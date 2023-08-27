/// @file main.h
///
/// @brief This file contains a getter method for the Lispy parser.

#pragma once

#ifndef LISPY_MAIN_H
#define LISPY_MAIN_H

#include "common.h"
#include "deps/mpc/mpc.h"

/// @brief Returns the lispy parser.
/// @return The lispy parser
mpc_parser_t* get_lispy_parser(void);

#endif
