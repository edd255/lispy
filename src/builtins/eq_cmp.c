#include "eq_cmp.h"

#include "../values.h"

//---- Equality comparison functions -------------------------------------------
lval_t* builtin_cmp(lenv_t* env, lval_t* a, char* operator) {
    assert(NULL != env);
    assert(NULL != a);
    assert(NULL != operator);
    UNUSED(env);
    LASSERT_NUM(operator, a, 2);

    int r = 0;
    if (0 == strcmp(operator, "==")) {
        r = lval_eq(a->cell[0], a->cell[1]);
    } else if (0 == strcmp(operator, "!=")) {
        r = !lval_eq(a->cell[0], a->cell[1]);
    } else {
        return lval_err("Error during comparison: Neither == nor != used!");
    }
    lval_del(a);
    return lval_num(r);
}

lval_t* builtin_eq(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);

    return builtin_cmp(env, a, "==");
}

lval_t* builtin_ne(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);

    return builtin_cmp(env, a, "!=");
}
