#include "builtins/env.h"

#include "builtins/list.h"
#include "eval.h"
#include "io.h"
#include "main.h"

Value* builtin_env(Environment* env, Value* args) {
    assert(NULL != env);
    assert(NULL != args);
    Value* x = val_qexpr();
    for (int i = 0; i < env->count; i++) {
        Value* y = val_sym(env->syms[i]);
        assert(NULL != y);
        x = val_add(x, y);
    }
    val_del(args);
    return x;
}

Value* builtin_fun(Environment* env, Value* args) {
    assert(NULL != env);
    assert(NULL != args);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_QEXPR);
    LCHECK_QEXPR_NOT_EMPTY(__func__, args, 0);
    LCHECK_QEXPR_NOT_EMPTY(__func__, args, 1);
    Value* fn_body = val_pop(args, 1);
    Value* fn_args = builtin_tail(env, val_copy(args));
    Value* fn_lambda = val_lambda(fn_args, fn_body);
    Value* fn_name = val_take(builtin_head(env, args), 0);
    env_def(env, fn_name, fn_lambda);
    val_del(fn_lambda);
    val_del(fn_name);
    return val_sexpr();
}

Value* builtin_load(Environment* env, Value* args) {
    assert(NULL != env);
    assert(NULL != args);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_STR);
    // Parse file given by string name
    mpc_result_t parse_result;
    mpc_parser_t* lispy = get_lispy_parser();
    if (mpc_parse_contents(args->cell[0]->str, lispy, &parse_result)) {
        // Read contents
        assert(NULL != parse_result.output);
        Value* expr = val_read(parse_result.output);
        assert(NULL != expr);
        mpc_ast_delete(parse_result.output);
        // Evaluate each expression
        while (expr->count) {
            Value* x = val_eval(env, val_pop(expr, 0));

            // If evaluation leads to error print it
            if (LISPY_VAL_ERR == x->type) {
                val_println(x);
            }
            val_del(x);
        }
        // Delete expressions and arguments
        val_del(expr);
        val_del(args);

        // Return empty list
        return val_sexpr();
    }
    // Get parse error as string
    char* err_msg = mpc_err_string(parse_result.error);
    mpc_err_delete(parse_result.error);
    // Create new error message using it
    Value* err = val_err("Could not load library %s", err_msg);
    FREE(err_msg);
    val_del(args);
    // Cleanup and return error
    return err;
}
