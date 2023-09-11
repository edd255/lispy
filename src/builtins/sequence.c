#include "builtins/sequence.h"

#include "eval.h"
#include "io.h"

lval* builtin_do(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);

    for (int i = 0; i < args->count; i++) {
        LCHECK_TYPE(__func__, args, i, LISPY_VAL_SEXPR);
    }
    for (int i = 0; i < args->count; i++) {
        (void)lval_eval(env, args->cell[i]);
    }
    lval_del(args);
    return lval_sexpr();
}
