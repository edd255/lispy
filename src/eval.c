#include "eval.h"

#include "printing.h"
#include "values.h"

//==== EVALUATION METHODS ======================================================

lval_t* lval_eval(lenv_t* e, lval_t* v) {
    assert(e != NULL);
    assert(v != NULL);

    if (v->type == LVAL_SYM) {
        lval_t* x = lenv_get(e, v);
        lval_del(v);
        return x;
    }
    // Evaluate Sexpressions
    if (v->type == LVAL_SEXPR) {
        return lval_eval_sexpr(e, v);
    }
    // All other lval_t types remain the same
    return v;
}

lval_t* lval_eval_sexpr(lenv_t* e, lval_t* v) {
    assert(e != NULL);
    assert(v != NULL);

    // Evaluate Children
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(e, v->cell[i]);
    }
    // Error Checking
    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == LVAL_ERR) {
            return lval_take(v, i);
        }
    }
    // Empty Expression
    if (v->count == 0) {
        return v;
    }
    // Single Expression
    if (v->count == 1) {
        return lval_take(v, 0);
    }
    // Ensure first element is a function after evaluation
    lval_t* f = lval_pop(v, 0);
    if (f->type != LVAL_FN) {
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
