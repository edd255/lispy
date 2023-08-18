#include "builtins.h"

#include <assert.h>

#include "common.h"
#include "eval.h"
#include "main.h"
#include "printing.h"
#include "reading.h"
#include "values.h"

//==== BUILTIN METHODS =========================================================

void lenv_add_builtins(lenv_t* e) {
    assert(NULL != e);

    // Variable functions
    lenv_add_builtin(e, "\\", builtin_lambda);
    lenv_add_builtin(e, "def", builtin_def);
    lenv_add_builtin(e, "=", builtin_put);

    // Environment functions
    lenv_add_builtin(e, "env", builtin_env);
    lenv_add_builtin(e, "fun", builtin_fun);

    // List functions
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "join", builtin_join);
    lenv_add_builtin(e, "cons", builtin_cons);
    lenv_add_builtin(e, "len", builtin_len);
    lenv_add_builtin(e, "pack", builtin_pack);
    lenv_add_builtin(e, "unpack", builtin_unpack);

    // Mathematical functions
    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);
    lenv_add_builtin(e, "%", builtin_mod);
    lenv_add_builtin(e, "^", builtin_pow);
    lenv_add_builtin(e, "max", builtin_max);
    lenv_add_builtin(e, "min", builtin_min);

    // Conditional functions
    lenv_add_builtin(e, "if", builtin_if);

    // Logical functions
    lenv_add_builtin(e, "and", builtin_and);
    lenv_add_builtin(e, "or", builtin_or);
    lenv_add_builtin(e, "not", builtin_not);

    // Comparison functions
    lenv_add_builtin(e, "==", builtin_eq);
    lenv_add_builtin(e, "!=", builtin_ne);
    lenv_add_builtin(e, ">", builtin_gt);
    lenv_add_builtin(e, "<", builtin_lt);
    lenv_add_builtin(e, ">=", builtin_ge);
    lenv_add_builtin(e, "<=", builtin_le);

    // String Functions
    lenv_add_builtin(e, "load", builtin_load);
    lenv_add_builtin(e, "error", builtin_error);
    lenv_add_builtin(e, "print", builtin_print);
}

void lenv_add_builtin(lenv_t* e, char* name, lbuiltin_t fn) {
    assert(NULL != e);
    assert(NULL != name);
    assert(NULL != fn);

    lval_t* k = lval_sym(name);
    lval_t* v = lval_fn(fn);
    lenv_put(e, k, v);
    lval_del(k);
    lval_del(v);
}
