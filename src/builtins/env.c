#include "builtins/env.h"

#include "builtins/list.h"
#include "io.h"

lval* builtin_env(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    lval* x = lval_qexpr();
    for (int i = 0; i < env->count; i++) {
        lval* y = lval_sym(env->syms[i]);
        assert(NULL != y);
        x = lval_add(x, y);
    }
    lval_del(args);
    return x;
}

lval* builtin_fun(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_QEXPR);
    LCHECK_QEXPR_NOT_EMPTY(__func__, args, 0);
    LCHECK_QEXPR_NOT_EMPTY(__func__, args, 1);
    lval* fn_body = lval_pop(args, 1);
    lval* fn_args = builtin_tail(env, lval_copy(args));
    lval* fn_lambda = lval_lambda(fn_args, fn_body);
    lval* fn_name = lval_take(builtin_head(env, args), 0);
    lenv_def(env, fn_name, fn_lambda);
    lval_del(fn_lambda);
    lval_del(fn_name);
    return lval_sexpr();
}
