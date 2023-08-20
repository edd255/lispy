#include "builtins/eq_cmp.h"

#include "values.h"

//---- Equality comparison functions -------------------------------------------
lval_t* builtin_cmp(lenv_t* env, lval_t* args, char* op) {
    assert(NULL != env);
    assert(NULL != args);
    assert(NULL != op);
    UNUSED(env);
    LASSERT_NUM(op, args, 2);

    int r = 0;
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

lval_t* builtin_eq(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_cmp(env, args, "==");
}

lval_t* builtin_ne(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_cmp(env, args, "!=");
}
