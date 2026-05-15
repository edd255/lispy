#include "builtins/magn_cmp.h"

#include "io.h"

//==== Comparison functions ====================================================
//---- Magnitude comparison functions ------------------------------------------
Value* builtin_ord(Environment* env, Value* args, char* op) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    ASSERT(op != NULL);
    UNUSED(env);
    LCHECK_NUM(op, args, 2);
    LCHECK_TYPE(op, args, 0, LISPY_VAL_NUM);
    LCHECK_TYPE(op, args, 1, LISPY_VAL_NUM);
    int r;
    if (strcmp(op, ">") == 0) {
        r = (args->cell[0]->num > args->cell[1]->num);
    } else if (strcmp(op, "<") == 0) {
        r = (args->cell[0]->num < args->cell[1]->num);
    } else if (strcmp(op, ">=") == 0) {
        r = (args->cell[0]->num >= args->cell[1]->num);
    } else if (strcmp(op, "<=") == 0) {
        r = (args->cell[0]->num <= args->cell[1]->num);
    } else {
        return val_err(
            "Error during magnitude comparison: Neither >, <, >=, <= used!"
        );
    }
    val_del(args);
    return val_num(r);
}

Value* builtin_gt(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_ord(env, args, ">");
}

Value* builtin_lt(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_ord(env, args, "<");
}

Value* builtin_ge(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_ord(env, args, ">=");
}

Value* builtin_le(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_ord(env, args, "<=");
}
