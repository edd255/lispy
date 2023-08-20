#include "builtins/env.h"

#include "builtins/list.h"
#include "printing.h"
#include "values.h"

lval_t* builtin_env(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);
    lval_t* x = lval_qexpr();
    for (int i = 0; i < env->count; i++) {
        lval_t* y = lval_sym(env->syms[i]);
        x = lval_add(x, y);
    }
    lval_del(a);
    return x;
}

lval_t* builtin_fun(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LVAL_QEXPR);
    LCHECK_TYPE(__func__, args, 1, LVAL_QEXPR);
    LCHECK_NOT_EMPTY(__func__, args, 0);
    LCHECK_NOT_EMPTY(__func__, args, 1);

    lval_t* fn_body = lval_pop(args, 1);
    lval_t* fn_args = builtin_tail(env, lval_copy(args));
    lval_t* fn_lambda = lval_lambda(fn_args, fn_body);
    lval_t* fn_name = lval_take(builtin_head(env, args), 0);
    lenv_def(env, fn_name, fn_lambda);
    lval_del(fn_lambda);
    lval_del(fn_name);
    return lval_sexpr();
}
