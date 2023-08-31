#include "builtins/conditional.h"

#include "builtins/list.h"
#include "deps/color-sh/color-sh.h"
#include "eval.h"
#include "io.h"

//==== Conditional functions ===================================================
lval_t* builtin_if(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 3);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_NUM, LISPY_VAL_SEXPR);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_QEXPR);
    LCHECK_TYPE(__func__, args, 2, LISPY_VAL_QEXPR);

    // Mark both expressions as evaluable
    lval_t* x = NULL;
    args->cell[1]->type = LISPY_VAL_SEXPR;
    args->cell[2]->type = LISPY_VAL_SEXPR;

    lval_t* cond = lval_eval(env, args->cell[0]);
    LCHECK(
        cond,
        cond->type == LISPY_VAL_NUM,
        "%s: symbolic expression did not evaluate to number",
        __func__
    );

    if (cond->num) {
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

lval_t* builtin_testhelper(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 3);

    lval_t* cond = args->cell[0];
    lval_t* expected = args->cell[1];
    lval_t* actual = args->cell[2];

    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_NUM);
    if (cond->num) {
        color_bgreen("PASSED\n");
    } else {
        color_bred("FAILED. ");
        printf("Expected: ");
        lval_print(expected);
        printf(". Got: ");
        lval_print(actual);
        printf("\n");
    }
    lval_del(args);
    return lval_sexpr();
}
