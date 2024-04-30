#include "builtins/strings.h"

#include "eval.h"
#include "io.h"
#include "main.h"

//==== String functions ========================================================
Value* builtin_print(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    for (int i = 0; i < args->count; i++) {
        val_print(args->cell[i]);
        putchar(' ');
    }
    putchar('\n');
    val_del(args);
    return val_sexpr();
}

Value* builtin_error(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_STR);
    Value* err = val_err(args->cell[0]->str);
    val_del(args);
    return err;
}
