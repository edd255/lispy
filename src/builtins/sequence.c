#include "builtins/sequence.h"

#include "eval.h"
#include "io.h"

Value* builtin_do(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    for (int i = 0; i < args->count; i++) {
        LCHECK_TYPES(__func__, args, i, LISPY_VAL_QEXPR, LISPY_VAL_SEXPR);
    }
    for (int i = 0; i < args->count; i++) {
        Value* expr = val_copy(args->cell[i]);
        expr->type = LISPY_VAL_SEXPR;
        Value* res = val_eval(env, expr);
        if (res->type == LISPY_VAL_ERR) {
            val_del(args);
            return res;
        }
        val_del(res);
    }
    val_del(args);
    return val_sexpr();
}
