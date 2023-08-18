#include "logic.h"

#include <assert.h>

#include "../printing.h"
#include "helpers.h"

//==== Logical functions =======================================================
lval_t* builtin_logic(lenv_t* e, lval_t* a, char* op) {
    assert(NULL != e);
    assert(NULL != a);
    UNUSED(e);
    int result = 0;
    switch (llogic_from_string(op)) {
        case LLOGIC_AND: {
            LASSERT_NUM(op, a, 2);
            LASSERT_TYPE(op, a, 0, LVAL_NUM);
            LASSERT_TYPE(op, a, 1, LVAL_NUM);
            if ((0 == a->cell[0]->num) || (0 == a->cell[1]->num)) {
                result = 0;
            } else {
                result = 1;
            }
            break;
        }
        case LLOGIC_OR: {
            LASSERT_NUM(op, a, 2);
            LASSERT_TYPE(op, a, 0, LVAL_NUM);
            LASSERT_TYPE(op, a, 1, LVAL_NUM);
            if ((0 == a->cell[0]->num) && (0 == a->cell[1]->num)) {
                result = 0;
            } else {
                result = 1;
            }
            break;
        }
        case LLOGIC_NOT: {
            LASSERT_NUM(op, a, 1);
            LASSERT_TYPE(op, a, 0, LVAL_NUM);
            if (0 == a->cell[0]->num) {
                result = 1;
            } else {
                result = 0;
            }
            break;
        }
        case LLOGIC_UNKNOWN: {
            return lval_err(
                "Function '%s' expected 'not', 'or' or 'and'",
                __func__
            );
        }
    }
    lval_del(a);
    return lval_num(result);
}

lval_t* builtin_and(lenv_t* e, lval_t* a) {
    return builtin_logic(e, a, "and");
}

lval_t* builtin_or(lenv_t* e, lval_t* a) {
    return builtin_logic(e, a, "or");
}

lval_t* builtin_not(lenv_t* e, lval_t* a) {
    return builtin_logic(e, a, "not");
}
