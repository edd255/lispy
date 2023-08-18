#include "magn_cmp.h"

#include <assert.h>

#include "../printing.h"

//==== Comparison functions ====================================================
//---- Magnitude comparison functions ------------------------------------------
lval_t* builtin_ord(lenv_t* e, lval_t* a, char* op) {
    assert(NULL != e);
    assert(NULL != a);
    assert(NULL != op);
    UNUSED(e);
    LASSERT_NUM(op, a, 2);
    LASSERT_TYPE(op, a, 0, LVAL_NUM);
    LASSERT_TYPE(op, a, 1, LVAL_NUM);

    int r = 0;
    if (0 == strcmp(op, ">")) {
        r = (a->cell[0]->num > a->cell[1]->num);
    } else if (0 == strcmp(op, "<")) {
        r = (a->cell[0]->num < a->cell[1]->num);
    } else if (0 == strcmp(op, ">=")) {
        r = (a->cell[0]->num >= a->cell[1]->num);
    } else if (0 == strcmp(op, "<=")) {
        r = (a->cell[0]->num <= a->cell[1]->num);
    } else {
        return lval_err(
            "Error during magnitude comparison: Neither >, <, >=, <= used!"
        );
    }
    lval_del(a);
    return lval_num(r);
}

lval_t* builtin_gt(lenv_t* e, lval_t* a) {
    assert(NULL != e);
    assert(NULL != a);

    return builtin_ord(e, a, ">");
}

lval_t* builtin_lt(lenv_t* e, lval_t* a) {
    assert(NULL != e);
    assert(NULL != a);

    return builtin_ord(e, a, "<");
}

lval_t* builtin_ge(lenv_t* e, lval_t* a) {
    assert(NULL != e);
    assert(NULL != a);

    return builtin_ord(e, a, ">=");
}

lval_t* builtin_le(lenv_t* e, lval_t* a) {
    assert(NULL != e);
    assert(NULL != a);

    return builtin_ord(e, a, "<=");
}
