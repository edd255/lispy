/// @file helpers.h
/// @brief This file contains all helper methods.

#pragma once

#ifndef LISPY_BUILTINS_HELPERS_H
#define LISPY_BUILTINS_HELPERS_H

#include <stdbool.h>

//==== HELPER METHODS ==========================================================
//---- Arithmetic methods ------------------------------------------------------
/// Enum representation of all implemented arithmetic operators
enum LISPY_ARITHM_OP {
    /// Addition
    LISPY_ARITHM_ADD,

    /// Subtraction
    LISPY_ARITHM_SUB,

    /// Multiplication
    LISPY_ARITHM_MUL,

    /// Division
    LISPY_ARITHM_DIV,

    /// Modular division
    LISPY_ARITHM_MOD,

    /// Exponentiation
    LISPY_ARITHM_POW,

    /// Maximum
    LISPY_ARITHM_MAX,

    /// Minimum
    LISPY_ARITHM_MIN,

    /// Unknown operator
    LISPY_ARITHM_UNKNOWN
};

/// @brief A data structure that maps string representations of arithmetic operators to their enum representation
typedef struct lispy_arithm_op_map_t lispy_arithm_op_map_t;

/// @brief A data structure that maps string representations of arithmetic operators to their enum representation
struct lispy_arithm_op_map_t {
    /// The string representation of the operator
    char* key;

    /// The enum representation of the operator
    enum LISPY_ARITHM_OP value;
};

/// A shorthand to calculate the number of implemented arithmetic operations
#define NUMBER_OF_ARITHM_OPS \
    (int)(sizeof(lispy_arithm_ops_map) / sizeof(lispy_arithm_op_map_t))

/// @brief Returns the enum representation of the arithmetic operator
/// @param key The string representation of the arithmetic operator
/// @return The enum representation of the arithmetic operator
enum LISPY_ARITHM_OP arithm_op_from_str(const char* key);

/// @brief Exponentiation method for long values
/// @param base The number to multiply
/// @param exponent The number of times to multiply
/// @return The resulting power
long power_long(long base, long exponent);

//---- Logical methods ---------------------------------------------------------
/// Enum representation of all implemented logical operators
enum LISPY_LOGIC_OP {
    /// And
    LISPY_LOGIC_AND,

    /// Or
    LISPY_LOGIC_OR,

    /// Not
    LISPY_LOGIC_NOT,

    /// Unknown
    LISPY_LOGIC_UNKNOWN
};

/// @brief A data structure that maps string representations of logic operators to their enum representation
typedef struct lispy_logic_op_map_t lispy_logic_op_map_t;

/// @brief A data structure that maps string representations of logic operators to their enum representation
struct lispy_logic_op_map_t {
    /// The string representation of the operator
    char* key;

    /// The enum representation of the operator
    enum LISPY_LOGIC_OP value;
};

/// A shorthand to calculate the number of implemented logical operations
#define NUMBER_OF_LOGIC_OPS \
    (int)(sizeof(lispy_logic_ops_map) / sizeof(lispy_logic_op_map_t))

/// @brief Returns the enum representation of the logical operator
/// @param key The string representation of the logical operator
/// @return The enum representation of the logical operator
enum LISPY_LOGIC_OP logic_op_from_str(const char* key);

#endif
