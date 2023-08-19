#include "builtins/vars.h"

#include "printing.h"
#include "values.h"

//==== Variable functions ======================================================
lval_t* builtin_var(lenv_t* env, lval_t* a, char* fn) {
    assert(NULL != env);
    assert(NULL != a);
    assert(NULL != fn);
    LASSERT_TYPE(fn, a, 0, LVAL_QEXPR);

    // First argument is symbol list
    lval_t* syms = a->cell[0];

    // Ensure all elements of first list are symbols
    for (int i = 0; i < syms->count; i++) {
        LASSERT(
            a,
            (LVAL_SYM == syms->cell[i]->type),
            "Error in procedure %s. Function '%s' cannot define non-symbol! Got %s. Expected %s.",
            __func__,
            fn,
            ltype_name(syms->cell[i]->type),
            ltype_name(LVAL_SYM)
        );
    }
    // Check correct number of symbols and values
    LASSERT(
        a,
        (a->count - 1 == syms->count),
        "Error in procedure %s. Function '%s' passed too many arguments for symbols. Got %i. Expected %i.",
        __func__,
        fn,
        syms->count,
        a->count - 1
    );
    // Assign copies of values to symbols
    for (int i = 0; i < syms->count; i++) {
        // If 'def' define in globally. If 'put' define in locally.
        if (0 == strcmp(fn, "def")) {
            lenv_def(env, syms->cell[i], a->cell[i + 1]);
        }
        if (0 == strcmp(fn, "=")) {
            lenv_put(env, syms->cell[i], a->cell[i + 1]);
        }
    }
    lval_del(a);
    return lval_sexpr();
}

lval_t* builtin_lambda(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);
    UNUSED(env);

    // Check two arguments, each of which are Q-Expressions
    LASSERT_NUM("\\", a, 2);
    LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
    LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

    // Check first Q-Expression contains only Symbols
    for (int i = 0; i < a->cell[0]->count; i++) {
        LASSERT(
            a,
            (LVAL_SYM == a->cell[0]->cell[i]->type),
            "Error in procedure %s. Cannot define non-symbol. Got %s. Expected %s.",
            __func__,
            ltype_name(a->cell[0]->cell[i]->type),
            ltype_name(LVAL_SYM)
        );
    }
    // Pop first two arguments and pass them to lval_lambda
    lval_t* formals = lval_pop(a, 0);
    lval_t* body = lval_pop(a, 0);

    assert(NULL != formals);
    assert(NULL != body);

    lval_del(a);
    return lval_lambda(formals, body);
}

lval_t* builtin_def(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);

    return builtin_var(env, a, "def");
}

lval_t* builtin_put(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);

    return builtin_var(env, a, "=");
}
