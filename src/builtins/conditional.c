#include "builtins/conditional.h"

#include "deps/color-sh/color-sh.h"
#include "eval.h"
#include "io.h"

//==== Conditional functions ===================================================
Value* builtin_if(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 3);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_NUM, LISPY_VAL_SEXPR);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_QEXPR);
    LCHECK_TYPE(__func__, args, 2, LISPY_VAL_QEXPR);
    // Mark both expressions as evaluable
    Value* x = NULL;
    args->cell[1]->type = LISPY_VAL_SEXPR;
    args->cell[2]->type = LISPY_VAL_SEXPR;
    Value* cond = val_eval(env, args->cell[0]);
    LCHECK(
        cond,
        cond->type == LISPY_VAL_NUM,
        "%s: symbolic expression did not evaluate to number",
        __func__
    );
    if (cond->num) {
        // If condition is true evaluate first expression
        Value* y = val_pop(args, 1);
        ASSERT(NULL != y);
        x = val_eval(env, y);
    } else {
        // Otherwise evaluate second expression
        Value* y = val_pop(args, 2);
        ASSERT(NULL != y);
        x = val_eval(env, y);
    }
    // Delete argument list and return
    val_del(args);
    return x;
}

Value* builtin_testhelper(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 3);
    const Value* cond = val_eval(env, args->cell[0]);
    Value* expected = args->cell[1];
    Value* actual = args->cell[2];
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_NUM);
    if (cond->num) {
        color_bgreen("PASSED\n");
    } else {
        color_bred("FAILED. ");
        printf("Expected: ");
        val_print(expected);
        printf(". Got: ");
        val_print(actual);
        printf("\n");
    }
    val_del(args);
    return val_sexpr();
}

Value* builtin_select(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    if (args->count == 0) {
        return val_err(
            "'%s' expected at least one selection but got 0",
            __func__
        );
    }
    for (int i = 0; i < args->count; i++) {
        LCHECK_NUM(__func__, args->cell[i], 2);
        Value* args_cond = val_copy(args->cell[i]->cell[0]);
        Value* body = val_copy(args->cell[i]->cell[1]);
        Value* cond = val_eval(env, args_cond);
        ASSERT(NULL != args_cond);
        ASSERT(NULL != body);
        ASSERT(NULL != cond);
        if (cond->num) {
            Value* res = val_eval(env, body);
            val_del(cond);
            val_del(args);
            return res;
        }
        val_del(cond);
        val_del(body);
    }
    val_del(args);
    return val_err(
        "'%s' expected that at least one selection is true but none is",
        __func__
    );
}

Value* builtin_case(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    const Value* arg = args->cell[0];
    for (int i = 1; i < args->count; i++) {
        Value* case_stmt = args->cell[i];
        int cond = val_eq(arg, case_stmt->cell[0]);
        if (cond) {
            Value* res = val_copy(case_stmt->cell[1]);
            val_del(args);
            return res;
        }
    }
    val_del(args);
    return val_err(
        "'%s' expected that arg matches one case but it doesn't",
        __func__
    );
}
