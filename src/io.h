#pragma once

#ifndef LISPY_IO_H
#define LISPY_IO_H

#include "common.h"
#include "deps/mpc/mpc.h"

//=== WRITING ==================================================================
/// @brief Prints the given Lispy value
/// @param val The value to print
void lval_print(lval_t* val);

/// @brief Prints the given expression
/// @param val The value to print
/// @param open For example, opening parenthesis
/// @param close For example, closing parenthesis
void lval_print_expr(lval_t* val, char open, char close);

/// @brief Prints the given Lispy value and included a newline
/// @param val The value to print
void lval_println(lval_t* val);

/// @brief Prints the given Lispy string
/// @param val The string to print
void lval_print_str(const lval_t* val);

/// @brief Returns the string representation of the given type
/// @param type The requested type
/// @returns The string representation of the given type
char* ltype_name(enum LVAL type);

//=== READING ==================================================================
/// @brief Reads an integer/decimal value from the abstract syntax tree.
/// @param tree The abstract syntax tree
/// @returns The Lispy value that contains the numerical value or an error
lval_t* lval_read_num(const mpc_ast_t* tree);

/// @brief Turns the given abstract syntax tree into an symbolic expression.
/// @param tree The abstract syntax tree
/// @returns An symbolic expression which represents the abstract syntax tree
lval_t* lval_read(mpc_ast_t* tree);

/// @brief Turns the given abstract syntax tree into a Lispy string
/// @param tree The abstract syntax tree
/// @returns The Lispy value that contains the string
lval_t* lval_read_str(mpc_ast_t* tree);

#endif
