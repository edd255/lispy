#include "builtins/eq_cmp.h"

//---- Equality comparison functions -------------------------------------------
Value* builtin_cmp(Environment* env, Value* args, char* op) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    ASSERT(op != NULL);
    UNUSED(env);
    LCHECK_NUM(op, args, 2);
    int r;
    if (strcmp(op, "==") == 0) {
        r = val_eq(args->cell[0], args->cell[1]);
    } else if (strcmp(op, "!=") == 0) {
        r = !val_eq(args->cell[0], args->cell[1]);
    } else {
        return val_err("Error during comparison: Neither == nor != used!");
    }
    val_del(args);
    return val_num(r);
}

Value* builtin_eq(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_cmp(env, args, "==");
}

Value* builtin_ne(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_cmp(env, args, "!=");
}
