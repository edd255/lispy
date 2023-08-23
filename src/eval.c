#include "eval.h"

#include "io.h"

//==== EVALUATION METHODS ======================================================

lval_t* lval_eval(lenv_t* env, lval_t* val) {
    assert(NULL != env);
    assert(NULL != val);

    if (LVAL_SYM == val->type) {
        lval_t* x = lenv_get(env, val);
        lval_del(val);
        return x;
    }
    // Evaluate Sexpressions
    if (LVAL_SEXPR == val->type) {
        return lval_eval_sexpr(env, val);
    }
    // All other lval_t types remain the same
    return val;
}

lval_t* lval_eval_sexpr(lenv_t* env, lval_t* val) {
    assert(NULL != env);
    assert(NULL != val);

    // Evaluate Children
    for (int i = 0; i < val->count; i++) {
        val->cell[i] = lval_eval(env, val->cell[i]);
    }
    // Error Checking
    for (int i = 0; i < val->count; i++) {
        if (LVAL_ERR == val->cell[i]->type) {
            return lval_take(val, i);
        }
    }
    // Empty Expression
    if (0 == val->count) {
        return val;
    }
    // Single Expression
    if (1 == val->count) {
        return lval_eval(env, lval_take(val, 0));
    }
    // Ensure first element is a function after evaluation
    lval_t* fn = lval_pop(val, 0);
    if (LVAL_FN != fn->type) {
        lval_t* err = lval_err(
            "S-Expression starts with incorrect type. Got %s, Expected %s.",
            ltype_name(fn->type),
            ltype_name(LVAL_FN)
        );
        lval_del(fn);
        lval_del(val);
        return err;
    }
    // Call builtin with op
    lval_t* result = lval_call(env, fn, val);
    lval_del(fn);
    return result;
}
