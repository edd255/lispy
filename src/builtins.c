#include "builtins.h"

#include "common.h"
#include "eval.h"
#include "main.h"
#include "printing.h"
#include "reading.h"
#include "values.h"

//==== BUILTIN METHODS =========================================================

void lenv_add_builtins(lenv_t* env) {
    assert(NULL != env);

    // Variable functions
    lenv_add_builtin(env, "\\", builtin_lambda);
    lenv_add_builtin(env, "def", builtin_def);
    lenv_add_builtin(env, "=", builtin_put);

    // Environment functions
    lenv_add_builtin(env, "env", builtin_env);
    lenv_add_builtin(env, "fun", builtin_fun);

    // List functions
    lenv_add_builtin(env, "list", builtin_list);
    lenv_add_builtin(env, "head", builtin_head);
    lenv_add_builtin(env, "tail", builtin_tail);
    lenv_add_builtin(env, "eval", builtin_eval);
    lenv_add_builtin(env, "join", builtin_join);
    lenv_add_builtin(env, "cons", builtin_cons);
    lenv_add_builtin(env, "len", builtin_len);
    lenv_add_builtin(env, "pack", builtin_pack);
    lenv_add_builtin(env, "unpack", builtin_unpack);

    // Mathematical functions
    lenv_add_builtin(env, "+", builtin_add);
    lenv_add_builtin(env, "-", builtin_sub);
    lenv_add_builtin(env, "*", builtin_mul);
    lenv_add_builtin(env, "/", builtin_div);
    lenv_add_builtin(env, "%", builtin_mod);
    lenv_add_builtin(env, "^", builtin_pow);
    lenv_add_builtin(env, "max", builtin_max);
    lenv_add_builtin(env, "min", builtin_min);

    // Conditional functions
    lenv_add_builtin(env, "if", builtin_if);

    // Logical functionvns
    lenv_add_builtin(env, "and", builtin_and);
    lenv_add_builtin(env, "or", builtin_or);
    lenv_add_builtin(env, "not", builtin_not);

    // Comparison funcnvtions
    lenv_add_builtin(env, "==", builtin_eq);
    lenv_add_builtin(env, "!=", builtin_ne);
    lenv_add_builtin(env, ">", builtin_gt);
    lenv_add_builtin(env, "<", builtin_lt);
    lenv_add_builtin(env, ">=", builtin_ge);
    lenv_add_builtin(env, "<=", builtin_le);

    // String Functionnvs
    lenv_add_builtin(env, "load", builtin_load);
    lenv_add_builtin(env, "error", builtin_error);
    lenv_add_builtin(env, "print", builtin_print);
}

void lenv_add_builtin(lenv_t* env, char* name, lbuiltin_t fn) {
    assert(NULL != env);
    assert(NULL != name);
    assert(NULL != fn);

    lval_t* key = lval_sym(name);
    lval_t* val = lval_fn(fn);
    lenv_put(env, key, val);
    lval_del(key);
    lval_del(val);
}
