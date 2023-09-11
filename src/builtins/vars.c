#include "builtins/vars.h"

#include "io.h"

//==== Variable functions ======================================================
lval* builtin_var(lenv* env, lval* args, char* fn) {
    assert(NULL != env);
    assert(NULL != args);
    assert(NULL != fn);
    LCHECK_TYPE(fn, args, 0, LISPY_VAL_QEXPR);

    // First argument is symbol list
    lval* syms = args->cell[0];

    // Ensure all elements of first list are symbols
    for (int i = 0; i < syms->count; i++) {
        LCHECK(
            args,
            (LISPY_VAL_SYM == syms->cell[i]->type),
            "Error in procedure %s. Function '%s' cannot define non-symbol! Got %s. Expected %s.",
            __func__,
            fn,
            ltype_name(syms->cell[i]->type),
            ltype_name(LISPY_VAL_SYM)
        );
    }
    // Check correct number of symbols and values
    LCHECK(
        args,
        (args->count - 1 == syms->count),
        "Error in procedure %s. Function '%s' passed too many arguments for symbols. Got %i. Expected %i.",
        __func__,
        fn,
        syms->count,
        args->count - 1
    );
    // Assign copies of values to symbols
    for (int i = 0; i < syms->count; i++) {
        // If 'def' define in globally. If 'put' define in locally.
        if (0 == strcmp(fn, "def")) {
            lenv_def(env, syms->cell[i], args->cell[i + 1]);
        }
        if (0 == strcmp(fn, "=")) {
            lenv_put(env, syms->cell[i], args->cell[i + 1]);
        }
    }
    lval_del(args);
    return lval_sexpr();
}

lval* builtin_lambda(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);

    // Check two arguments, each of which are Q-Expressions
    LCHECK_NUM("\\", args, 2);
    LCHECK_TYPE("\\", args, 0, LISPY_VAL_QEXPR);
    LCHECK_TYPE("\\", args, 1, LISPY_VAL_QEXPR);

    // Check first Q-Expression contains only Symbols
    for (int i = 0; i < args->cell[0]->count; i++) {
        LCHECK(
            args,
            (LISPY_VAL_SYM == args->cell[0]->cell[i]->type),
            "Error in procedure %s. Cannot define non-symbol. Got %s. Expected %s.",
            __func__,
            ltype_name(args->cell[0]->cell[i]->type),
            ltype_name(LISPY_VAL_SYM)
        );
    }
    // Pop first two arguments and pass them to lval_lambda
    lval* formals = lval_pop(args, 0);
    lval* body = lval_pop(args, 0);

    assert(NULL != formals);
    assert(NULL != body);

    lval_del(args);
    return lval_lambda(formals, body);
}

lval* builtin_def(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_var(env, args, "def");
}

lval* builtin_put(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_var(env, args, "=");
}
