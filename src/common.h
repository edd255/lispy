/// @file common.h
///
/// @brief This file contains the definitions, constructors, destructors and
/// methods for Lispy values and environments.

#pragma once

#ifndef LISPY_COMMON_H
#define LISPY_COMMON_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/alloc.h"
#include "utils/errors.h"

/// Generic bufsize definition
enum { BUFSIZE = 512 };


//=== DECLARATIONS =============================================================
//--- Structs ------------------------------------------------------------------
/// A data structure that contains Lispy values.
struct lval_t;

/// A data structure that contains Lispy values.
typedef struct lval_t lval_t;

/// A structure that contains data and metadata about used environments.
struct lenv_t;

/// A structure that contains data and metadata about used environments.
typedef struct lenv_t lenv_t;

/// A function pointer that takes an lenv_t* and a lval_t* and returns a lval_t*, briefly: a pointer to a builtin function.
typedef lval_t* (*lbuiltin_t)(lenv_t*, lval_t*);

/// @brief A data structure that contains Lispy values.
///
/// The data structure contains the type and the associated numeric, string,
/// functional or expressional value.
struct lval_t {
    //--- Type -----------------------------------------------------------------
    /// The type of the value
    int type;

    //--- Basic ----------------------------------------------------------------
    /// The integer value, stored as long
    long num;

    /// The decimal value, stored as double
    double dec;

    /// The string for Lispy errors
    char* err;

    /// Symbol types have somge string data, too
    char* sym;

    /// The string value for Lispy strings
    char* str;

    //--- Function -------------------------------------------------------------
    /// A pointer to the function
    lbuiltin_t builtin;

    /// The environment of the function
    lenv_t* env;

    /// The formal arguments
    lval_t* formals;

    /// The body of the function
    lval_t* body;

    //--- Expression -----------------------------------------------------------

    /// The number of values in the list
    int count;

    /// A pointer to the list
    lval_t** cell;
};

/// @brief A structure that contains data and metadata about used environments.
///
/// The env_t struct encodes a list of relationships between symbols (names) and
/// values. This is implemented by using two lists of equal length, where each
/// entry in one list has a corresponding entry in the other list at the same
/// position.
struct lenv_t {
    /// A pointer to the parent environment
    lenv_t* parent;

    /// The number of symbols in the environment
    int count;

    /// A pointer to the symbols
    char** syms;

    /// A pointer to the values
    lval_t** vals;
};

//--- Constructors & Destructors for Values ------------------------------------
/// @brief Constructor for numeric values
/// @param value The numeric value
/// @return A Lispy value that contains the given numeric value
lval_t* lval_num(long value);

/// @brief Constructor for decimal values
/// @param value The decimal value
/// @return A Lispy value that contains the given decimal value
lval_t* lval_dec(double value);

/// @brief Constructor for error values
/// @param fmt The error message
/// @param ... The format specifiers used in the error message
/// @return A Lispy value that contains the error
lval_t* lval_err(char* fmt, ...);

/// @brief Constructor for symbolic values
/// @param str String data which the symbolic value should contain
/// @return A Lispy value that contains the symbolic value
lval_t* lval_sym(char* str);

/// @brief Constructor for symbolic expressions
/// @return A Lispy value that contains an empty symbolic expression
lval_t* lval_sexpr(void);

/// @brief Constructor for quoted expressions
/// @return A Lispy value that contains an empty quoted expression
lval_t* lval_qexpr(void);

/// @brief Constructor for functional values
/// @param fn A function pointer to the builtin method
/// @return A Lispy value that contains the builtin method
lval_t* lval_fn(lbuiltin_t fn);

/// @brief Constructor for lambda expressions
/// @param formals The formal arguments of the lambda expression
/// @param body The body of the function
/// @return A Lispy value that contains the lambda function
lval_t* lval_lambda(lval_t* formals, lval_t* body);

/// @brief Constructor for string values
/// @param str String data
/// @return A Lispy value that contains the given string
lval_t* lval_str(const char* str);

//--- Methods for Values -------------------------------------------------------

/// @brief Copies a Lispy value and returns a new one which contains the given
/// value.
/// @param self The value to copy
/// @return A Lispy value that is equal to self
lval_t* lval_copy(const lval_t* self);

/// @brief Adds an element to a symbolic expression.
///
/// @param self The symbolic expression
/// @param other The value to add
/// @return A pointer to the symbolic expression which includes x
lval_t* lval_add(lval_t* self, lval_t* other);

/// @brief Adds each cell in x to self
///
/// This function iteratively calls lval_add to join to symbolic expressions.
///
/// @param self The first symbolic expression
/// @param other The second symbolic expression
/// @return A Lispy value that is the join of both symbolic expressions
lval_t* lval_join(lval_t* self, lval_t* other);

/// @brief Pops the value at index idx from self.
///
/// \todo The method behaves undefined if idx >= self->count.
///
/// @param self The symbolic expression to pop the value from.
/// @param idx The index at which the value resides.
/// @return The requested value at index idx.
lval_t* lval_pop(lval_t* self, int idx);

/// @brief As pop, but deletes self.
///
/// @param self The symbolic expression to pop the value from.
/// @param idx The index at which the value resides.
/// @return The requested value at index idx.
lval_t* lval_take(lval_t* self, int idx);

/// @brief Calls a function with specific arguments in an environment.
/// Differentiates between builtins and lambdas.
/// @param env The environment to call the function in
/// @param fn The function to call
/// @param args The arguments to use for the function call
/// @return A (partial) evaluation of the function call
lval_t* lval_call(lenv_t* env, lval_t* fn, lval_t* args);

/// @brief Compares to Lispy values.
/// @param self The first Lispy value to compare
/// @param other The second Lispy value to compare
/// @return 1 if both are equal, else 0.
int lval_eq(const lval_t* self, const lval_t* other);

/// @brief Deletes a Lispy value
/// @param self The value to free
void lval_del(lval_t* self);

//--- Constructors & Destructors for Environments ------------------------------

/// @brief Creates a new, empty environment.
/// @return A pointer to the created environment.
lenv_t* lenv_new(void);

/// @brief Frees the given environment.
/// @param env A pointer to the environment
void lenv_del(lenv_t* env);

//--- Methods for Environments -------------------------------------------------

/// @brief Returns the associated value for the given key in the environment.
/// @param env The environment to lookup
/// @param key The key for the value
/// @return The value associated with the key in the environment
lval_t* lenv_get(lenv_t* env, lval_t* key);

/// @brief Puts key and value in the environment
/// @param env The environment in which key and value should be put
/// @param key The key for the value
/// @param val The associated value
void lenv_put(lenv_t* env, const lval_t* key, const lval_t* val);

/// @brief Copies an environment to a new one
/// @param env The environment to copy
/// @return A pointer to the newly created environment
lenv_t* lenv_copy(lenv_t* env);

/// @brief Defines a key-value pair globally.
///
/// This method defines a key-value pair globally by chaining up the parent
/// environment until it achieves the root environment.
///
/// @param env The given environment
/// @param key The key to define
/// @param val The associated value
void lenv_def(lenv_t* env, const lval_t* key, const lval_t* val);

//=== ENUMS ====================================================================
/// An enumeration of possible error types
enum LISPY_ERR {
    /// Division by Zero
    LISPY_ERR_DIV_BY_ZERO,

    /// Bad operator
    LISPY_ERR_BAD_OP,

    /// Bad number
    LISPY_ERR_BAD_NUM
};

/// Enumeration of possible Lispy value types
enum LISPY_VAL {
    /// Error type
    LISPY_VAL_ERR,

    /// Integer number
    LISPY_VAL_NUM,

    /// Decimal number
    LISPY_VAL_DEC,

    /// Symbol
    LISPY_VAL_SYM,

    /// String
    LISPY_VAL_STR,

    /// Symbol expression
    LISPY_VAL_SEXPR,

    /// Quoted expression
    LISPY_VAL_QEXPR,

    /// Function
    LISPY_VAL_FN
};

#endif
