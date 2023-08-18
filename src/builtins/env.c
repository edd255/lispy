#include "env.h"

#include "../printing.h"
#include "../values.h"
#include "list.h"

lval_t* builtin_env(lenv_t* e, lval_t* a) {
    assert(NULL != e);
    assert(NULL != a);
    lval_t* x = lval_qexpr();
    for (int i = 0; i < e->count; i++) {
        lval_t* y = lval_sym(e->syms[i]);
        x = lval_add(x, y);
    }
    lval_del(a);
    return x;
}

lval_t* builtin_fun(lenv_t* e, lval_t* a) {
    assert(NULL != e);
    assert(NULL != a);
    LASSERT_NUM(__func__, a, 2);
    LASSERT_TYPE(__func__, a, 0, LVAL_QEXPR);
    LASSERT_TYPE(__func__, a, 1, LVAL_QEXPR);
    LASSERT_NOT_EMPTY(__func__, a, 0);
    LASSERT_NOT_EMPTY(__func__, a, 1);

    lval_t* body = lval_pop(a, 1);
    lval_t* args = builtin_tail(e, lval_copy(a));
    lval_t* lambda = lval_lambda(args, body);
    lval_t* name = lval_take(builtin_head(e, a), 0);
    lenv_def(e, name, lambda);
    lval_del(lambda);
    lval_del(name);
    return lval_sexpr();
}
