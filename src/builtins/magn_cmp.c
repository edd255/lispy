#include "builtins/magn_cmp.h"

#include "printing.h"

//==== Comparison functions ====================================================
//---- Magnitude comparison functions ------------------------------------------
lval_t* builtin_ord(lenv_t* env, lval_t* args, char* op) {
    assert(NULL != env);
    assert(NULL != args);
    assert(NULL != op);
    UNUSED(env);
    LASSERT_NUM(op, args, 2);
    LASSERT_TYPE(op, args, 0, LVAL_NUM);
    LASSERT_TYPE(op, args, 1, LVAL_NUM);

    int r = 0;
    if (0 == strcmp(op, ">")) {
        r = (args->cell[0]->num > args->cell[1]->num);
    } else if (0 == strcmp(op, "<")) {
        r = (args->cell[0]->num < args->cell[1]->num);
    } else if (0 == strcmp(op, ">=")) {
        r = (args->cell[0]->num >= args->cell[1]->num);
    } else if (0 == strcmp(op, "<=")) {
        r = (args->cell[0]->num <= args->cell[1]->num);
    } else {
        return lval_err(
            "Error during magnitude comparison: Neither >, <, >=, <= used!"
        );
    }
    lval_del(args);
    return lval_num(r);
}

lval_t* builtin_gt(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_ord(env, args, ">");
}

lval_t* builtin_lt(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_ord(env, args, "<");
}

lval_t* builtin_ge(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_ord(env, args, ">=");
}

lval_t* builtin_le(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_ord(env, args, "<=");
}
