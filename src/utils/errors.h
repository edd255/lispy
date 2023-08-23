#pragma once

#ifndef LISPY_UTILS_ERRORS_H
#define LISPY_UTILS_ERRORS_H

//=== ERRORS ===================================================================
#define LCHECK(args, cond, fmt, ...) \
    if (!(cond)) { \
        lval_t* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(args); \
        return err; \
    }

#define LCHECKS(args, cond1, cond2, fmt, ...) \
    if (!((cond1) || (cond2))) { \
        lval_t* err = lval_err((fmt), ##__VA_ARGS__); \
        lval_del(args); \
        return err; \
    }

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

#define LCHECK_TYPES(fn, args, idx, expect1, expect2) \
    LCHECKS( \
        (args), \
        (args)->cell[idx]->type == ((expect1)), \
        (args)->cell[idx]->type == ((expect2)), \
        "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s or %s.", \
        (fn), \
        (idx), \
        ltype_name((args)->cell[idx]->type), \
        ltype_name(expect1), \
        ltype_name(expect2) \
    )

#define LCHECK_NUM(fn, args, num) \
    LCHECK( \
        (args), \
        (args)->count == ((num)), \
        "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
        (fn), \
        (args)->count, \
        (num) \
    )

#define LCHECK_NOT_EMPTY(fn, args, idx) \
    LCHECK( \
        (args), \
        (args)->cell[idx]->count != 0, \
        "Function '%s' passed  for argument %i.", \
        (fn), \
        (idx) \
    )

#define UNUSED(x) (void)(x)

#endif
