#include "builtins.h"

//==== BUILTIN METHODS =========================================================

void lenv_add_builtins(lenv_t* env) {
    assert(NULL != env);

    // Variable functions
    lenv_add_builtin_fn(env, "\\", builtin_lambda);
    lenv_add_builtin_fn(env, "def", builtin_def);
    lenv_add_builtin_fn(env, "=", builtin_put);

    // Environment functions
    lenv_add_builtin_fn(env, "env", builtin_env);
    lenv_add_builtin_fn(env, "fun", builtin_fun);

    // List functions
    lenv_add_builtin_fn(env, "list", builtin_list);
    lenv_add_builtin_fn(env, "head", builtin_head);
    lenv_add_builtin_fn(env, "tail", builtin_tail);
    lenv_add_builtin_fn(env, "eval", builtin_eval);
    lenv_add_builtin_fn(env, "join", builtin_join);
    lenv_add_builtin_fn(env, "cons", builtin_cons);
    lenv_add_builtin_fn(env, "len", builtin_len);
    lenv_add_builtin_fn(env, "pack", builtin_pack);
    lenv_add_builtin_fn(env, "unpack", builtin_unpack);
    lenv_add_builtin_fn(env, "curry", builtin_unpack);
    lenv_add_builtin_fn(env, "uncurry", builtin_pack);
    lenv_add_builtin_fn(env, "nth", builtin_nth);
    lenv_add_builtin_fn(env, "fst", builtin_first);
    lenv_add_builtin_fn(env, "snd", builtin_second);
    lenv_add_builtin_fn(env, "trd", builtin_third);
    lenv_add_builtin_fn(env, "last", builtin_last);
    lenv_add_builtin_fn(env, "elem", builtin_elem);
    lenv_add_builtin_fn(env, "init", builtin_init);

    // Sequence functions
    lenv_add_builtin_fn(env, "do", builtin_do);

    // Mathematical functions
    lenv_add_builtin_fn(env, "+", builtin_add);
    lenv_add_builtin_fn(env, "-", builtin_sub);
    lenv_add_builtin_fn(env, "*", builtin_mul);
    lenv_add_builtin_fn(env, "/", builtin_div);
    lenv_add_builtin_fn(env, "%", builtin_mod);
    lenv_add_builtin_fn(env, "^", builtin_pow);
    lenv_add_builtin_fn(env, "max", builtin_max);
    lenv_add_builtin_fn(env, "min", builtin_min);
    lenv_add_builtin_fn(env, "sum", builtin_sum);
    lenv_add_builtin_fn(env, "prod", builtin_prod);

    // Conditional functions
    lenv_add_builtin_fn(env, "if", builtin_if);
    lenv_add_builtin_fn(env, "testhelper", builtin_testhelper);
    lenv_add_builtin_fn(env, "select", builtin_select);

    // Logical functionvns
    lenv_add_builtin_fn(env, "and", builtin_and);
    lenv_add_builtin_fn(env, "or", builtin_or);
    lenv_add_builtin_fn(env, "not", builtin_not);

    // Comparison funcnvtions
    lenv_add_builtin_fn(env, "==", builtin_eq);
    lenv_add_builtin_fn(env, "!=", builtin_ne);
    lenv_add_builtin_fn(env, ">", builtin_gt);
    lenv_add_builtin_fn(env, "<", builtin_lt);
    lenv_add_builtin_fn(env, ">=", builtin_ge);
    lenv_add_builtin_fn(env, "<=", builtin_le);

    // String Functions
    lenv_add_builtin_fn(env, "load", builtin_load);
    lenv_add_builtin_fn(env, "error", builtin_error);
    lenv_add_builtin_fn(env, "print", builtin_print);

    // Atoms
    lenv_add_builtin_const(env, "nil", lval_qexpr());
    lenv_add_builtin_const(env, "true", lval_num(1));
    lenv_add_builtin_const(env, "false", lval_num(0));
    lenv_add_builtin_const(env, "otherwise", lval_num(1));
}

void lenv_add_builtin_fn(lenv_t* env, char* name, lbuiltin_t fn) {
    assert(NULL != env);
    assert(NULL != name);
    assert(NULL != fn);

    lval_t* key = lval_sym(name);
    lval_t* value = lval_fn(fn);
    lenv_put(env, key, value);
    lval_del(key);
    lval_del(value);
}

void lenv_add_builtin_const(lenv_t* env, char* name, lval_t* value) {
    assert(NULL != env);
    assert(NULL != name);
    assert(NULL != value);

    lval_t* key = lval_sym(name);
    lenv_put(env, key, value);
    lval_del(key);
    lval_del(value);
}
