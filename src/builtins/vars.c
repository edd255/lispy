#include "builtins/vars.h"

#include "io.h"

//==== Variable functions ======================================================
Value* builtin_var(Environment* env, Value* args, char* fn) {
    assert(NULL != env);
    assert(NULL != args);
    assert(NULL != fn);
    LCHECK_TYPE(fn, args, 0, LISPY_VAL_QEXPR);
    // First argument is symbol list
    Value* syms = args->cell[0];
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
            env_def(env, syms->cell[i], args->cell[i + 1]);
        }
        if (0 == strcmp(fn, "=")) {
            env_put(env, syms->cell[i], args->cell[i + 1]);
        }
    }
    val_del(args);
    return val_sexpr();
}

Value* builtin_lambda(Environment* env, Value* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    // Check two arguments, each of which are Q-Expressions
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_QEXPR);
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
    // Pop first two arguments and pass them to val_lambda
    Value* formals = val_pop(args, 0);
    Value* body = val_pop(args, 0);
    assert(NULL != formals);
    assert(NULL != body);
    val_del(args);
    return val_lambda(formals, body);
}

Value* builtin_def(Environment* env, Value* args) {
    assert(NULL != env);
    assert(NULL != args);
    return builtin_var(env, args, "def");
}

Value* builtin_put(Environment* env, Value* args) {
    assert(NULL != env);
    assert(NULL != args);
    return builtin_var(env, args, "=");
}
