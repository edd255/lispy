#include "eval.h"

#include "io.h"

//==== EVALUATION METHODS ======================================================

Value* val_eval(Environment* env, Value* val) {
    ASSERT(NULL != env);
    ASSERT(NULL != val);
    // Evaluate symbols
    if (LISPY_VAL_SYM == val->type) {
        Value* x = env_get(env, val);
        val_del(val);
        return x;
    }
    // Evaluate symbolic expressions
    if (LISPY_VAL_SEXPR == val->type) {
        return val_eval_sexpr(env, val);
    }
    // All other types remain the same
    return val;
}

Value* val_eval_sexpr(Environment* env, Value* val) {
    ASSERT(NULL != env);
    ASSERT(NULL != val);
    // Evaluate Children
    for (int i = 0; i < val->count; i++) {
        val->cell[i] = val_eval(env, val->cell[i]);
    }
    // Error Checking
    for (int i = 0; i < val->count; i++) {
        if (LISPY_VAL_ERR == val->cell[i]->type) {
            return val_take(val, i);
        }
    }
    // Empty Expression
    if (0 == val->count) {
        return val;
    }
    // Single Expression
    if (1 == val->count) {
        return val_eval(env, val_take(val, 0));
    }
    // Ensure first element is a function after evaluation
    Value* fn = val_pop(val, 0);
    if (LISPY_VAL_FN != fn->type) {
        Value* err = val_err(
            "S-Expression starts with incorrect type. Got %s, Expected %s.",
            ltype_name(fn->type),
            ltype_name(LISPY_VAL_FN)
        );
        val_del(fn);
        val_del(val);
        return err;
    }
    // Call builtin with op
    Value* result = val_call(env, fn, val);
    val_del(fn);
    return result;
}
