#include "eq_cmp.h"

#include "../values.h"

//---- Equality comparison functions -------------------------------------------
lval_t* builtin_cmp(lenv_t* e, lval_t* a, char* op) {
    assert(NULL != e);
    assert(NULL != a);
    assert(NULL != op);
    UNUSED(e);
    LASSERT_NUM(op, a, 2);

    int r;
    if (0 == strcmp(op, "==")) {
        r = lval_eq(a->cell[0], a->cell[1]);
    } else if (0 == strcmp(op, "!=")) {
        r = !lval_eq(a->cell[0], a->cell[1]);
    } else {
        return lval_err("Error during comparison: Neither == nor != used!");
    }
    lval_del(a);
    return lval_num(r);
}

lval_t* builtin_eq(lenv_t* e, lval_t* a) {
    assert(NULL != e);
    assert(NULL != a);

    return builtin_cmp(e, a, "==");
}

lval_t* builtin_ne(lenv_t* e, lval_t* a) {
    assert(NULL != e);
    assert(NULL != a);

    return builtin_cmp(e, a, "!=");
}
