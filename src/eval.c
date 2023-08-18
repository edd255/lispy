#include "eval.h"

#include <assert.h>

#include "printing.h"
#include "values.h"

//==== EVALUATION METHODS ======================================================

lval_t* lval_eval(lenv_t* e, lval_t* v) {
    assert(NULL != e);
    assert(NULL != v);

    if (LVAL_SYM == v->type) {
        lval_t* x = lenv_get(e, v);
        lval_del(v);
        return x;
    }
    // Evaluate Sexpressions
    if (LVAL_SEXPR == v->type) {
        return lval_eval_sexpr(e, v);
    }
    // All other lval_t types remain the same
    return v;
}

lval_t* lval_eval_sexpr(lenv_t* e, lval_t* v) {
    assert(NULL != e);
    assert(NULL != v);

    // Evaluate Children
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(e, v->cell[i]);
    }
    // Error Checking
    for (int i = 0; i < v->count; i++) {
        if (LVAL_ERR == v->cell[i]->type) {
            return lval_take(v, i);
        }
    }
    // Empty Expression
    if (0 == v->count) {
        return v;
    }
    // Single Expression
    if (1 == v->count) {
        return lval_eval(e, lval_take(v, 0));
    }
    // Ensure first element is a function after evaluation
    lval_t* f = lval_pop(v, 0);
    if (LVAL_FN != f->type) {
        lval_t* err = lval_err(
            "S-Expression starts with incorrect type. Got %s, Expected %s.",
            ltype_name(f->type),
            ltype_name(LVAL_FN)
        );
        lval_del(f);
        lval_del(v);
        return err;
    }
    // Call builtin with operator
    lval_t* result = lval_call(e, f, v);
    lval_del(f);
    return result;
}
