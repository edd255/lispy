#include "builtins/magn_cmp.h"

#include "printing.h"

//==== Comparison functions ====================================================
//---- Magnitude comparison functions ------------------------------------------
lval_t* builtin_ord(lenv_t* env, lval_t* a, char* operator) {
    assert(NULL != env);
    assert(NULL != a);
    assert(NULL != operator);
    UNUSED(env);
    LASSERT_NUM(operator, a, 2);
    LASSERT_TYPE(operator, a, 0, LVAL_NUM);
    LASSERT_TYPE(operator, a, 1, LVAL_NUM);

    int r = 0;
    if (0 == strcmp(operator, ">")) {
        r = (a->cell[0]->num > a->cell[1]->num);
    } else if (0 == strcmp(operator, "<")) {
        r = (a->cell[0]->num < a->cell[1]->num);
    } else if (0 == strcmp(operator, ">=")) {
        r = (a->cell[0]->num >= a->cell[1]->num);
    } else if (0 == strcmp(operator, "<=")) {
        r = (a->cell[0]->num <= a->cell[1]->num);
    } else {
        return lval_err(
            "Error during magnitude comparison: Neither >, <, >=, <= used!"
        );
    }
    lval_del(a);
    return lval_num(r);
}

lval_t* builtin_gt(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);

    return builtin_ord(env, a, ">");
}

lval_t* builtin_lt(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);

    return builtin_ord(env, a, "<");
}

lval_t* builtin_ge(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);

    return builtin_ord(env, a, ">=");
}

lval_t* builtin_le(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);

    return builtin_ord(env, a, "<=");
}
