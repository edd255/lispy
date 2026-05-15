#include "builtins/misc.h"

#include "common.h"
#include "io.h"
#include "utils/errors.h"

//==== Miscellaneous functions =================================================
Value* builtin_flip(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    LCHECK_NUM(__func__, args, 3);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_FN);
    Value* f = val_copy(args->cell[0]);
    Value* f_args = val_sexpr();
    val_add(f_args, val_copy(args->cell[2]));
    val_add(f_args, val_copy(args->cell[1]));
    Value* res = val_call(env, f, f_args);
    val_del(f);
    val_del(args);
    return res;
}

Value* builtin_comp(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    LCHECK_NUM(__func__, args, 3);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_FN);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_FN);
    Value* g = val_copy(args->cell[1]);
    Value* g_args = val_add(val_sexpr(), val_copy(args->cell[2]));
    Value* gx = val_call(env, g, g_args);
    val_del(g);
    if (gx->type == LISPY_VAL_ERR) {
        val_del(args);
        return gx;
    }
    Value* f = val_copy(args->cell[0]);
    Value* f_args = val_add(val_sexpr(), gx);
    Value* res = val_call(env, f, f_args);
    val_del(f);
    val_del(args);
    return res;
}
