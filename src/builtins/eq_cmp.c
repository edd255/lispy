#include "builtins/eq_cmp.h"

//---- Equality comparison functions -------------------------------------------
Value* builtin_cmp(Environment* env, Value* args, char* op) {
    assert(NULL != env);
    assert(NULL != args);
    assert(NULL != op);
    UNUSED(env);
    LCHECK_NUM(op, args, 2);
    int r;
    if (0 == strcmp(op, "==")) {
        r = val_eq(args->cell[0], args->cell[1]);
    } else if (0 == strcmp(op, "!=")) {
        r = !val_eq(args->cell[0], args->cell[1]);
    } else {
        return val_err("Error during comparison: Neither == nor != used!");
    }
    val_del(args);
    return val_num(r);
}

Value* builtin_eq(Environment* env, Value* args) {
    assert(NULL != env);
    assert(NULL != args);
    return builtin_cmp(env, args, "==");
}

Value* builtin_ne(Environment* env, Value* args) {
    assert(NULL != env);
    assert(NULL != args);
    return builtin_cmp(env, args, "!=");
}
