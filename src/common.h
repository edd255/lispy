/// @file common.h
///
/// @brief This file contains the definitions, constructors, destructors and
/// methods for Lispy values and environments.

#pragma once

#include <math.h>
#ifndef LISPY_COMMON_H
#define LISPY_COMMON_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deps/logger/log.h"
#include "utils/alloc.h"
#include "utils/errors.h"

/// Buffer size definition
enum {
    /// Buffers of size 1024 are used
    BUFSIZE = 1024
};

//=== ASSERT ===================================================================
#ifndef NDEBUG
/// Assert macro that logs failed assertions fatally & exits application
#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            log_fatal("Assertion failed: %s", #condition); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#else
#define ASSERT(condition)
#endif

//=== DECLARATIONS =============================================================
//--- Structs ------------------------------------------------------------------
/// A data structure that contains Lispy values.
struct Value;

/// A data structure that contains Lispy values.
typedef struct Value Value;

/// A structure that contains data and metadata about used environments.
struct Environment;

/// A structure that contains data and metadata about used environments.
typedef struct Environment Environment;

/// A function pointer that takes an Environment* and a Value* and returns a Value*, briefly: a pointer to a builtin function.
typedef Value* (*Function)(Environment*, Value*);

/// @brief A data structure that contains Lispy values.
///
/// The data structure contains the type and the associated numeric, string,
/// functional or expressional value.
struct Value {
    //--- Type -----------------------------------------------------------------
    /// The type of the value
    int type;

    //--- Basic ----------------------------------------------------------------
    /// The integer value, stored as long
    long num;

    /// The decimal value, stored as double
    double dec;

    //--- String ---------------------------------------------------------------
    /// The string for Lispy errors
    char* err;

    /// Symbol types have some string data, too
    char* sym;

    /// The string value for Lispy strings
    char* str;

    /// The length of the string, without NULL terminator
    size_t len;

    //--- Function -------------------------------------------------------------
    /// A pointer to the function
    Function builtin;

    /// The environment of the function
    Environment* env;

    /// The formal arguments
    Value* formals;

    /// The body of the function
    Value* body;

    //--- Expression -----------------------------------------------------------
    /// The number of values in the list
    int count;

    /// A pointer to the list
    Value** cell;
};

/// @brief A structure that contains data and metadata about used environments.
///
/// The env_t struct encodes a list of relationships between symbols (names) and
/// values. This is implemented by using two lists of equal length, where each
/// entry in one list has a corresponding entry in the other list at the same
/// position.
struct Environment {
    /// A pointer to the parent environment
    Environment* parent;

    /// The number of symbols in the environment
    int count;

    /// A pointer to the symbols
    char** syms;

    /// A pointer to the values
    Value** vals;
};

//--- Constructors & Destructors for Values ------------------------------------
/// @brief Constructor for numeric values
/// @param value The numeric value
/// @return A Lispy value that contains the given numeric value
Value* val_num(long value);

/// @brief Constructor for decimal values
/// @param value The decimal value
/// @return A Lispy value that contains the given decimal value
Value* val_dec(double value);

/// @brief Constructor for error values
/// @param fmt The error message
/// @param ... The format specifiers used in the error message
/// @return A Lispy value that contains the error
Value* val_err(char* fmt, ...);

/// @brief Constructor for symbolic values
/// @param str String data which the symbolic value should contain
/// @return A Lispy value that contains the symbolic value
Value* val_sym(char* str);

/// @brief Constructor for string values
/// @param str String data
/// @return A Lispy value that contains the given string
Value* val_str(const char* str);

/// @brief Constructor for symbolic expressions
/// @return A Lispy value that contains an empty symbolic expression
Value* val_sexpr(void);

/// @brief Constructor for quoted expressions
/// @return A Lispy value that contains an empty quoted expression
Value* val_qexpr(void);

/// @brief Constructor for functional values
/// @param fn A function pointer to the builtin method
/// @return A Lispy value that contains the builtin method
Value* val_fn(Function fn);

/// @brief Constructor for lambda expressions
/// @param formals The formal arguments of the lambda expression
/// @param body The body of the function
/// @return A Lispy value that contains the lambda function
Value* val_lambda(Value* formals, Value* body);

//--- Methods for Values -------------------------------------------------------

/// @brief Copies a Lispy value and returns a new one which contains the given
/// value.
/// @param self The value to copy
/// @return A Lispy value that is equal to self
Value* val_copy(const Value* self);

/// @brief Adds an element to a symbolic expression.
///
/// @param self The symbolic expression
/// @param other The value to add
/// @return A pointer to the symbolic expression which includes x
Value* val_add(Value* self, Value* other);

/// @brief Adds each cell in x to self
///
/// This function iteratively calls val_add to join to symbolic expressions.
///
/// @param self The first symbolic expression
/// @param other The second symbolic expression
/// @return A Lispy value that is the join of both symbolic expressions
Value* val_join(Value* self, Value* other);

/// @brief Pops the value at index idx from self.
///
/// \todo The method behaves undefined if idx >= self->count.
///
/// @param self The symbolic expression to pop the value from.
/// @param idx The index at which the value resides.
/// @return The requested value at index idx.
Value* val_pop(Value* self, int idx);

/// @brief As pop, but deletes self.
///
/// @param self The symbolic expression to pop the value from.
/// @param idx The index at which the value resides.
/// @return The requested value at index idx.
Value* val_take(Value* self, int idx);

/// @brief Calls a function with specific arguments in an environment.
/// Differentiates between builtins and lambdas.
/// @param env The environment to call the function in
/// @param fn The function to call
/// @param args The arguments to use for the function call
/// @return A (partial) evaluation of the function call
Value* val_call(Environment* env, Value* fn, Value* args);

/// @brief Compares to Lispy values.
/// @param self The first Lispy value to compare
/// @param other The second Lispy value to compare
/// @return 1 if both are equal, else 0.
int val_eq(const Value* self, const Value* other);

/// @brief Deletes a Lispy value
/// @param self The value to free
void val_del(Value* self);

//--- Constructors & Destructors for Environments ------------------------------

/// @brief Creates a new, empty environment.
/// @return A pointer to the created environment.
Environment* env_new(void);

/// @brief Frees the given environment.
/// @param env A pointer to the environment
void env_del(Environment* env);

//--- Methods for Environments -------------------------------------------------

/// @brief Returns the associated value for the given key in the environment.
/// @param env The environment to lookup
/// @param key The key for the value
/// @return The value associated with the key in the environment
Value* env_get(Environment* env, Value* key);

/// @brief Puts key and value in the environment
/// @param env The environment in which key and value should be put
/// @param key The key for the value
/// @param val The associated value
void env_put(Environment* env, const Value* key, const Value* val);

/// @brief Copies an environment to a new one
/// @param env The environment to copy
/// @return A pointer to the newly created environment
Environment* env_copy(Environment* env);

/// @brief Defines a key-value pair globally.
///
/// This method defines a key-value pair globally by chaining up the parent
/// environment until it achieves the root environment.
///
/// @param env The given environment
/// @param key The key to define
/// @param val The associated value
void env_def(Environment* env, const Value* key, const Value* val);

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
