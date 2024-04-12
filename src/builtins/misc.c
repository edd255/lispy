#include "builtins/misc.h"

#include "common.h"
#include "io.h"
#include "utils/errors.h"

//==== Miscellaneous functions =================================================
Value* builtin_flip(Environment* env, Value* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 3);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_FN);
    LCHECK_TYPE(__func__, args, 2, LISPY_VAL_FN);
    Value* f = val_copy(args->cell[0]);
    Value* f_args = val_qexpr();
    val_add(f_args, val_copy(args->cell[2]));
    val_add(f_args, val_copy(args->cell[1]));
    Value* res = val_call(env, f, f_args);
    val_del(args);
    return res;
}

Value* builtin_comp(Environment* env, Value* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 3);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_FN);
    LCHECK_TYPE(__func__, args, 2, LISPY_VAL_FN);
    Value* f = args->cell[0];
    Value* g = args->cell[1];
    Value* x = args->cell[2];
    Value* res = val_call(env, f, val_call(env, g, x));
    val_del(args);
    return res;
}
