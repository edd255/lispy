#include "builtins/sequence.h"

#include "eval.h"
#include "io.h"

Value* builtin_do(Environment* env, Value* args) {
    assert(NULL != env);
    assert(NULL != args);
    for (int i = 0; i < args->count; i++) {
        LCHECK_TYPE(__func__, args, i, LISPY_VAL_SEXPR);
    }
    for (int i = 0; i < args->count; i++) {
        (void)val_eval(env, args->cell[i]);
    }
    val_del(args);
    return val_sexpr();
}
