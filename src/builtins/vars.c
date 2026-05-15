#include "builtins/vars.h"

#include "io.h"

//==== Variable functions ======================================================
Value* builtin_var(Environment* env, Value* args, char* fn) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    ASSERT(fn != NULL);
    LCHECK_TYPE(fn, args, 0, LISPY_VAL_QEXPR);
    // First argument is symbol list
    Value* syms = args->cell[0];
    // Ensure all elements of first list are symbols
    for (int i = 0; i < syms->count; i++) {
        LCHECK(
            args,
            (syms->cell[i]->type == LISPY_VAL_SYM),
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
        (syms->count == args->count - 1),
        "Error in procedure %s. Function '%s' passed too many arguments for symbols. Got %i. Expected %i.",
        __func__,
        fn,
        syms->count,
        args->count - 1
    );
    // Assign copies of values to symbols
    for (int i = 0; i < syms->count; i++) {
        // If 'def' define in globally. If 'put' define in locally.
        if (strcmp(fn, "def") == 0) {
            env_def(env, syms->cell[i], args->cell[i + 1]);
        }
        if (strcmp(fn, "=") == 0) {
            env_put(env, syms->cell[i], args->cell[i + 1]);
        }
    }
    val_del(args);
    return val_sexpr();
}

Value* builtin_lambda(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    // Check two arguments, each of which are Q-Expressions
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_QEXPR);
    // Check first Q-Expression contains only Symbols
    for (int i = 0; i < args->cell[0]->count; i++) {
        LCHECK(
            args,
            (args->cell[0]->cell[i]->type == LISPY_VAL_SYM),
            "Error in procedure %s. Cannot define non-symbol. Got %s. Expected %s.",
            __func__,
            ltype_name(args->cell[0]->cell[i]->type),
            ltype_name(LISPY_VAL_SYM)
        );
    }
    // Pop first two arguments and pass them to val_lambda
    Value* formals = val_pop(args, 0);
    Value* body = val_pop(args, 0);
    ASSERT(formals != NULL);
    ASSERT(body != NULL);
    val_del(args);
    return val_lambda(env, formals, body);
}

Value* builtin_def(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_var(env, args, "def");
}

Value* builtin_put(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_var(env, args, "=");
}
