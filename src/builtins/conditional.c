#include "builtins/conditional.h"

#include "deps/color-sh/color-sh.h"
#include "eval.h"
#include "io.h"

//==== Conditional functions ===================================================
Value* builtin_if(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    LCHECK_NUM(__func__, args, 3);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_NUM, LISPY_VAL_SEXPR);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_QEXPR);
    LCHECK_TYPE(__func__, args, 2, LISPY_VAL_QEXPR);
    // Mark both expressions as evaluable
    Value* x = NULL;
    args->cell[1]->type = LISPY_VAL_SEXPR;
    args->cell[2]->type = LISPY_VAL_SEXPR;
    Value* cond = val_eval(env, val_copy(args->cell[0]));
    if (cond->type == LISPY_VAL_ERR) {
        val_del(args);
        return cond;
    }
    if (cond->type != LISPY_VAL_NUM) {
        Value* err = val_err(
            "%s: symbolic expression did not evaluate to number",
            __func__
        );
        val_del(cond);
        val_del(args);
        return err;
    }
    if (cond->num) {
        // If condition is true evaluate first expression
        Value* y = val_pop(args, 1);
        ASSERT(y != NULL);
        x = val_eval(env, y);
    } else {
        // Otherwise evaluate second expression
        Value* y = val_pop(args, 2);
        ASSERT(y != NULL);
        x = val_eval(env, y);
    }
    val_del(cond);
    // Delete argument list and return
    val_del(args);
    return x;
}

Value* builtin_testhelper(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
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
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    if (args->count == 0) {
        Value* err =
            val_err("'%s' expected at least one selection but got 0", __func__);
        val_del(args);
        return err;
    }
    for (int i = 0; i < args->count; i++) {
        Value* selection = args->cell[i];
        LCHECK(
            args,
            selection->type == LISPY_VAL_QEXPR,
            "'%s' expected a qexpr at index %d but got %s",
            __func__,
            i,
            ltype_name(selection->type)
        );
        LCHECK(
            args,
            selection->count == 2,
            "'%s' expected 2 elements at index %d but got %d",
            __func__,
            i,
            selection->count
        );
        Value* args_cond = val_copy(selection->cell[0]);
        Value* body = val_copy(selection->cell[1]);
        Value* cond = val_eval(env, args_cond);
        ASSERT(args_cond != NULL);
        ASSERT(body != NULL);
        ASSERT(cond != NULL);
        if (cond->type == LISPY_VAL_ERR) {
            val_del(body);
            val_del(args);
            return cond;
        }
        if (cond->type != LISPY_VAL_NUM) {
            Value* err = val_err(
                "'%s' expected condition at index %d to evaluate to number",
                __func__,
                i
            );
            val_del(cond);
            val_del(body);
            val_del(args);
            return err;
        }
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
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    UNUSED(env);
    LCHECK(
        args,
        args->count >= 2,
        "'%s' expected a value and at least one case but got %d arguments",
        __func__,
        args->count
    );
    const Value* arg = args->cell[0];
    for (int i = 1; i < args->count; i++) {
        Value* case_stmt = args->cell[i];
        LCHECK(
            args,
            case_stmt->type == LISPY_VAL_QEXPR,
            "'%s' expected a qexpr at index %d but got %s",
            __func__,
            i,
            ltype_name(case_stmt->type)
        );
        LCHECK(
            args,
            case_stmt->count == 2,
            "'%s' expected 2 elements at index %d but got %d",
            __func__,
            i,
            case_stmt->count
        );
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
