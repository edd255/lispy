#include "vars.h"

#include "../printing.h"
#include "../values.h"

//==== Variable functions ======================================================
lval_t* builtin_var(lenv_t* e, lval_t* a, char* fn) {
    assert(e != NULL);
    assert(a != NULL);
    assert(fn != NULL);
    LASSERT_TYPE(fn, a, 0, LVAL_QEXPR)

    // First argument is symbol list
    lval_t* syms = a->cell[0];

    // Ensure all elements of first list are symbols
    for (int i = 0; i < syms->count; i++) {
        LASSERT(
            a,
            (syms->cell[i]->type == LVAL_SYM),
            "Error in procedure %s. Function '%s' cannot define non-symbol! Got %s. Expected %s.",
            __func__,
            fn,
            ltype_name(syms->cell[i]->type),
            ltype_name(LVAL_SYM)
        )
    }
    // Check correct number of symbols and values
    LASSERT(
        a,
        (syms->count == a->count - 1),
        "Error in procedure %s. Function '%s' passed too many arguments for symbols. Got %i. Expected %i.",
        __func__,
        fn,
        syms->count,
        a->count - 1
    )
    // Assign copies of values to symbols
    for (int i = 0; i < syms->count; i++) {
        // If 'def' define in globally. If 'put' define in locally.
        if (strcmp(fn, "def") == 0) {
            lenv_def(e, syms->cell[i], a->cell[i + 1]);
        }
        if (strcmp(fn, "=") == 0) {
            lenv_put(e, syms->cell[i], a->cell[i + 1]);
        }
    }
    lval_del(a);
    return lval_sexpr();
}

lval_t* builtin_lambda(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);
    UNUSED(e);

    // Check two arguments, each of which are Q-Expressions
    LASSERT_NUM("\\", a, 2)
    LASSERT_TYPE("\\", a, 0, LVAL_QEXPR)
    LASSERT_TYPE("\\", a, 1, LVAL_QEXPR)

    // Check first Q-Expression contains only Symbols
    for (int i = 0; i < a->cell[0]->count; i++) {
        LASSERT(
            a,
            (a->cell[0]->cell[i]->type == LVAL_SYM),
            "Error in procedure %s. Cannot define non-symbol. Got %s. Expected %s.",
            __func__,
            ltype_name(a->cell[0]->cell[i]->type),
            ltype_name(LVAL_SYM)
        )
    }
    // Pop first two arguments and pass them to lval_lambda
    lval_t* formals = lval_pop(a, 0);
    lval_t* body = lval_pop(a, 0);

    assert(formals != NULL);
    assert(body != NULL);

    lval_del(a);
    return lval_lambda(formals, body);
}

lval_t* builtin_def(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_var(e, a, "def");
}

lval_t* builtin_put(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_var(e, a, "=");
}