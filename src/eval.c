#include "eval.h"

#include "io.h"

//==== EVALUATION METHODS ======================================================

lval* lval_eval(lenv* env, lval* val) {
    assert(NULL != env);
    assert(NULL != val);

    // Evaluate symbols
    if (LISPY_VAL_SYM == val->type) {
        lval* x = lenv_get(env, val);
        lval_del(val);
        return x;
    }
    // Evaluate symbolic expressions
    if (LISPY_VAL_SEXPR == val->type) {
        return lval_eval_sexpr(env, val);
    }
    // All other types remain the same
    return val;
}

lval* lval_eval_sexpr(lenv* env, lval* val) {
    assert(NULL != env);
    assert(NULL != val);

    // Evaluate Children
    for (int i = 0; i < val->count; i++) {
        val->cell[i] = lval_eval(env, val->cell[i]);
    }
    // Error Checking
    for (int i = 0; i < val->count; i++) {
        if (LISPY_VAL_ERR == val->cell[i]->type) {
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
    lval* fn = lval_pop(val, 0);
    if (LISPY_VAL_FN != fn->type) {
        lval* err = lval_err(
            "S-Expression starts with incorrect type. Got %s, Expected %s.",
            ltype_name(fn->type),
            ltype_name(LISPY_VAL_FN)
        );
        lval_del(fn);
        lval_del(val);
        return err;
    }
    // Call builtin with op
    lval* result = lval_call(env, fn, val);
    lval_del(fn);
    return result;
}
