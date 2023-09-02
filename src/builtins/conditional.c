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

    const lval_t* cond = lval_eval(env, args->cell[0]);
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

lval_t* builtin_select(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    if (args->count == 0) {
        return lval_err(
            "'%s' expected at least one selection but got 0",
            __func__
        );
    }
    for (int i = 0; i < args->count; i++) {
        LCHECK_NUM(__func__, args->cell[i], 2);
        lval_t* args_cond = lval_copy(args->cell[i]->cell[0]);
        lval_t* body = lval_copy(args->cell[i]->cell[1]);
        lval_t* cond = lval_eval(env, args_cond);
        assert(NULL != args_cond);
        assert(NULL != body);
        assert(NULL != cond);
        if (cond->num) {
            lval_t* res = lval_eval(env, body);
            lval_del(cond);
            lval_del(args);
            return res;
        }
        lval_del(cond);
        lval_del(body);
    }
    lval_del(args);
    return lval_err(
        "'%s' expected that at least one selection is true but none is",
        __func__
    );
}

lval_t* builtin_case(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);

    lval_t* arg = args->cell[0];
    for (int i = 1; i < args->count; i++) {
        lval_t* case_stmt = args->cell[i];
        int cond = lval_eq(arg, case_stmt->cell[0]);
        if (cond) {
            lval_t* res = lval_copy(case_stmt->cell[1]);
            lval_del(args);
            return res;
        }
    }
    lval_del(args);
    return lval_err(
        "'%s' expected that arg matches one case but it doesn't",
        __func__
    );
}
