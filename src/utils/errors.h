/// @file errors.h
/// @brief This file contains macros to check runtime assertions and do runtime
/// error reporting

#pragma once

#ifndef LISPY_UTILS_ERRORS_H
#define LISPY_UTILS_ERRORS_H

//=== ERRORS ===================================================================
/// @brief Macro to check an assertion and report errors during runtime
#define LCHECK(args, cond, fmt, ...) \
    if (!(cond)) { \
        Value* err = val_err(fmt, ##__VA_ARGS__); \
        val_del(args); \
        return err; \
    }

/// @brief Macro to check types and report errors during runtime
#define LCHECK_TYPE(fn, args, idx, expect) \
    LCHECK( \
        (args), \
        (args)->cell[idx]->type == ((expect)), \
        "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
        (fn), \
        (idx), \
        ltype_name((args)->cell[idx]->type), \
        ltype_name(expect) \
    )

/// @brief Macro to check two assertion and report errors during runtime
#define LCHECK_TYPES(fn, args, idx, expect1, expect2) \
    LCHECK( \
        (args), \
        ((args)->cell[idx]->type == ((expect1)) \
         || (args)->cell[idx]->type == ((expect2))), \
        "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s or %s.", \
        (fn), \
        (idx), \
        ltype_name((args)->cell[idx]->type), \
        ltype_name(expect1), \
        ltype_name(expect2) \
    )

/// @brief Macro to check whether an index is smaller than a list size
#define LCHECK_IDX_QEXPR(fn, args, cell_idx, idx) \
    LCHECK( \
        (args), \
        ((idx) < ((args)->cell[cell_idx]->count)), \
        "Function '%s' passed index %d but argument has size %d.", \
        (fn), \
        (idx), \
        ((args)->count) \
    )

/// @brief Macro to check whether an index is smaller than a string length
#define LCHECK_IDX_STR(fn, args, cell_idx, idx) \
    LCHECK( \
        (args), \
        ((idx) < ((args)->cell[cell_idx]->len)), \
        "Function '%s' passed index %d but argument has size %d.", \
        (fn), \
        (idx), \
        ((args)->cell[(cell_idx)]->len) \
    )

/// @brief Macro to check whether a function passed the correct number of
/// arguments
#define LCHECK_NUM(fn, args, num) \
    LCHECK( \
        (args), \
        (num) == (args)->count, \
        "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
        (fn), \
        (args)->count, \
        (num) \
    )

/// @brief Macro to check whether a quoted expression is empty and report it as
/// error
#define LCHECK_QEXPR_NOT_EMPTY(fn, args, idx) \
    LCHECK( \
        (args), \
        (0 != (args)->cell[idx]->count), \
        "Function '%s' passed {} for argument %i.", \
        (fn), \
        (idx) \
    )

/// @brief Macro to check whether a string is empty and report it as error
#define LCHECK_STR_NOT_EMPTY(fn, args, idx) \
    LCHECK( \
        (args), \
        (0 != (args)->cell[idx]->len), \
        "Function '%s' passed {} for argument %i.", \
        (fn), \
        (idx) \
    )

/// @brief Macro to avoid compiler warnings about unused variables
#define UNUSED(x) (void)(x)

#endif
