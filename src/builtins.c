#include "builtins.h"

#include "builtins/arithmetic.h"
#include "builtins/conditional.h"
#include "builtins/env.h"
#include "builtins/eq_cmp.h"
#include "builtins/list.h"
#include "builtins/logic.h"
#include "builtins/magn_cmp.h"
#include "builtins/misc.h"
#include "builtins/sequence.h"
#include "builtins/strings.h"
#include "builtins/vars.h"

//==== BUILTIN METHODS =========================================================

void env_add_builtins(Environment* env) {
    assert(NULL != env);
    // Variable functions
    env_add_builtin_fn(env, "lambda", builtin_lambda);
    env_add_builtin_fn(env, "def", builtin_def);
    env_add_builtin_fn(env, "=", builtin_put);
    // Environment functions
    env_add_builtin_fn(env, "env", builtin_env);
    env_add_builtin_fn(env, "fn", builtin_fun);
    env_add_builtin_fn(env, "load", builtin_load);
    // List functions
    env_add_builtin_fn(env, "list", builtin_list);
    env_add_builtin_fn(env, "head", builtin_head);
    env_add_builtin_fn(env, "tail", builtin_tail);
    env_add_builtin_fn(env, "eval", builtin_eval);
    env_add_builtin_fn(env, "join", builtin_join);
    env_add_builtin_fn(env, "cons", builtin_cons);
    env_add_builtin_fn(env, "len", builtin_len);
    env_add_builtin_fn(env, "pack", builtin_pack);
    env_add_builtin_fn(env, "unpack", builtin_unpack);
    env_add_builtin_fn(env, "curry", builtin_unpack);
    env_add_builtin_fn(env, "uncurry", builtin_pack);
    env_add_builtin_fn(env, "nth", builtin_nth);
    env_add_builtin_fn(env, "fst", builtin_first);
    env_add_builtin_fn(env, "snd", builtin_second);
    env_add_builtin_fn(env, "trd", builtin_third);
    env_add_builtin_fn(env, "last", builtin_last);
    env_add_builtin_fn(env, "elem", builtin_elem);
    env_add_builtin_fn(env, "init", builtin_init);
    env_add_builtin_fn(env, "take", builtin_take);
    env_add_builtin_fn(env, "drop", builtin_drop);
    env_add_builtin_fn(env, "split", builtin_split);
    env_add_builtin_fn(env, "filter", builtin_filter);
    env_add_builtin_fn(env, "reverse", builtin_reverse);
    env_add_builtin_fn(env, "map", builtin_map);
    env_add_builtin_fn(env, "lookup", builtin_lookup);
    // Sequence functions
    env_add_builtin_fn(env, "do", builtin_do);
    // Mathematical functions
    env_add_builtin_fn(env, "+", builtin_add);
    env_add_builtin_fn(env, "-", builtin_sub);
    env_add_builtin_fn(env, "*", builtin_mul);
    env_add_builtin_fn(env, "/", builtin_div);
    env_add_builtin_fn(env, "%", builtin_mod);
    env_add_builtin_fn(env, "^", builtin_pow);
    env_add_builtin_fn(env, "max", builtin_max);
    env_add_builtin_fn(env, "min", builtin_min);
    env_add_builtin_fn(env, "sum", builtin_sum);
    env_add_builtin_fn(env, "prod", builtin_prod);
    // Conditional functions
    env_add_builtin_fn(env, "if", builtin_if);
    env_add_builtin_fn(env, "testhelper", builtin_testhelper);
    env_add_builtin_fn(env, "select", builtin_select);
    env_add_builtin_fn(env, "case", builtin_case);
    // Logical functions
    env_add_builtin_fn(env, "and", builtin_and);
    env_add_builtin_fn(env, "or", builtin_or);
    env_add_builtin_fn(env, "not", builtin_not);
    // Comparison functions
    env_add_builtin_fn(env, "==", builtin_eq);
    env_add_builtin_fn(env, "!=", builtin_ne);
    env_add_builtin_fn(env, ">", builtin_gt);
    env_add_builtin_fn(env, "<", builtin_lt);
    env_add_builtin_fn(env, ">=", builtin_ge);
    env_add_builtin_fn(env, "<=", builtin_le);
    // String Functions
    env_add_builtin_fn(env, "error", builtin_error);
    env_add_builtin_fn(env, "print", builtin_print);
    // Misc
    env_add_builtin_fn(env, "flip", builtin_flip);
    env_add_builtin_fn(env, "comp", builtin_comp);
    // Atoms
    env_add_builtin_const(env, "nil", val_qexpr());
    env_add_builtin_const(env, "true", val_num(1));
    env_add_builtin_const(env, "false", val_num(0));
    env_add_builtin_const(env, "otherwise", val_num(1));
}

void env_add_builtin_fn(Environment* env, char* name, Function fn) {
    assert(NULL != env);
    assert(NULL != name);
    assert(NULL != fn);
    Value* key = val_sym(name);
    Value* value = val_fn(fn);
    env_put(env, key, value);
    val_del(key);
    val_del(value);
}

void env_add_builtin_const(Environment* env, char* name, Value* value) {
    assert(NULL != env);
    assert(NULL != name);
    assert(NULL != value);
    Value* key = val_sym(name);
    env_put(env, key, value);
    val_del(key);
    val_del(value);
}
