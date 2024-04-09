#include "builtins/env.h"

#include "builtins/list.h"
#include "io.h"

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
