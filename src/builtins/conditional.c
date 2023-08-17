#include "conditional.h"

#include "../eval.h"
#include "../printing.h"
#include "../values.h"

//==== Conditional functions ===================================================
lval_t* builtin_if(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);
    UNUSED(e);
    LASSERT_NUM(__func__, a, 3)
    LASSERT_TYPE(__func__, a, 0, LVAL_NUM)
    LASSERT_TYPE(__func__, a, 1, LVAL_QEXPR)
    LASSERT_TYPE(__func__, a, 2, LVAL_QEXPR)

    // Mark both expressions as evaluable
    lval_t* x;
    a->cell[1]->type = LVAL_SEXPR;
    a->cell[2]->type = LVAL_SEXPR;

    if (a->cell[0]->num) {
        // If condition is true evaluate first expression
        lval_t* y = lval_pop(a, 1);
        assert(y != NULL);
        x = lval_eval(e, y);
    } else {
        // Otherwise evaluate second expression
        lval_t* y = lval_pop(a, 2);
        assert(y != NULL);
        x = lval_eval(e, y);
    }
    // Delete argument list and return
    lval_del(a);
    return x;
}
