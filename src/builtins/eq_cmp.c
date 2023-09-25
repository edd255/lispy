#include "builtins/eq_cmp.h"

//---- Equality comparison functions -------------------------------------------
lval* builtin_cmp(lenv* env, lval* args, char* op) {
    assert(NULL != env);
    assert(NULL != args);
    assert(NULL != op);
    UNUSED(env);
    LCHECK_NUM(op, args, 2);
    int r;
    if (0 == strcmp(op, "==")) {
        r = lval_eq(args->cell[0], args->cell[1]);
    } else if (0 == strcmp(op, "!=")) {
        r = !lval_eq(args->cell[0], args->cell[1]);
    } else {
        return lval_err("Error during comparison: Neither == nor != used!");
    }
    lval_del(args);
    return lval_num(r);
}

lval* builtin_eq(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    return builtin_cmp(env, args, "==");
}

lval* builtin_ne(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    return builtin_cmp(env, args, "!=");
}
