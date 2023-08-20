#include "builtins/conditional.h"

#include "eval.h"
#include "printing.h"
#include "values.h"

//==== Conditional functions ===================================================
lval_t* builtin_if(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LASSERT_NUM(__func__, args, 3);
    LASSERT_TYPE(__func__, args, 0, LVAL_NUM);
    LASSERT_TYPE(__func__, args, 1, LVAL_QEXPR);
    LASSERT_TYPE(__func__, args, 2, LVAL_QEXPR);

    // Mark both expressions as evaluable
    lval_t* x = NULL;
    args->cell[1]->type = LVAL_SEXPR;
    args->cell[2]->type = LVAL_SEXPR;

    if (args->cell[0]->num) {
        // If condition is true evaluate first expression
        lval_t* y = lval_pop(args, 1);
        assert(NULL != y);
        x = lval_eval(env, y);
    } else {
        // Otherwise evaluate second expression
        lval_t* y = lval_pop(args, 2);
        assert(NULL != y);
        x = lval_eval(env, y);
    }
    // Delete argument list and return
    lval_del(args);
    return x;
}
