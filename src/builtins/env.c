#include "builtins/env.h"

#include "printing.h"
#include "values.h"
#include "builtins/list.h"

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

lval_t* builtin_fun(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);
    LASSERT_NUM(__func__, a, 2);
    LASSERT_TYPE(__func__, a, 0, LVAL_QEXPR);
    LASSERT_TYPE(__func__, a, 1, LVAL_QEXPR);
    LASSERT_NOT_EMPTY(__func__, a, 0);
    LASSERT_NOT_EMPTY(__func__, a, 1);

    lval_t* body = lval_pop(a, 1);
    lval_t* args = builtin_tail(env, lval_copy(a));
    lval_t* lambda = lval_lambda(args, body);
    lval_t* name = lval_take(builtin_head(env, a), 0);
    lenv_def(env, name, lambda);
    lval_del(lambda);
    lval_del(name);
    return lval_sexpr();
}
