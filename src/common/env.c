#include "common.h"

//=== ENVIRONMENT ==============================================================
lenv* lenv_new(void) {
    lenv* env = MALLOC(sizeof(lenv));
    env->parent = NULL;
    env->count = 0;
    env->syms = NULL;
    env->vals = NULL;
    return env;
}

void lenv_del(lenv* env) {
    assert(NULL != env);

    for (int i = 0; i < env->count; i++) {
        FREE(env->syms[i]);
        lval_del(env->vals[i]);
    }
    FREE(env->syms);
    FREE(env->vals);
    FREE(env);
}

lval* lenv_get(lenv* env, lval* key) {
    assert(NULL != env);
    assert(NULL != key);

    // Iterate over all items in environment
    for (int i = 0; i < env->count; i++) {
        // Check if the stored string matches the symbol string. If it does,
        // return a copy of the value
        if (0 == strcmp(env->syms[i], key->sym)) {
            return lval_copy(env->vals[i]);
        }
    }
    // If no symbol check in parent otherwise error
    if (env->parent) {
        return lenv_get(env->parent, key);
    }
    return lval_err("Unbound symbol '%s'", key->sym);
}

void lenv_put(lenv* env, const lval* key, const lval* val) {
    assert(NULL != env);
    assert(NULL != key);
    assert(NULL != val);

    // Iterate over all items in environment. This is to see if variable already
    // exists.
    for (int i = 0; i < env->count; i++) {
        // If variable is found delete item at that position. And replace with
        // variable supplied by user.
        if (0 == strcmp(env->syms[i], key->sym)) {
            lval_del(env->vals[i]);
            env->vals[i] = lval_copy(val);
            return;
        }
    }
    // If no existing entry found allocate space for new entry
    env->count++;
    env->vals = REALLOC(env->vals, sizeof(lval*) * env->count);
    env->syms = REALLOC(env->syms, sizeof(char*) * env->count);

    // Copy contents of lval and symbol string into new location
    env->vals[env->count - 1] = lval_copy(val);
    env->syms[env->count - 1] = MALLOC(key->len + 1);

    strlcpy(env->syms[env->count - 1], key->sym, key->len + 1);
}

lenv* lenv_copy(lenv* env) {
    assert(NULL != env);

    lenv* n = MALLOC(sizeof(lenv));
    n->parent = env->parent;
    n->count = env->count;
    n->syms = MALLOC(sizeof(char*) * (n->count));
    n->vals = MALLOC(sizeof(lval*) * (n->count));
    for (int i = 0; i < env->count; i++) {
        size_t env_syms_len = strlen(env->syms[i] + 1);
        n->syms[i] = MALLOC(env_syms_len);
        strlcpy(n->syms[i], env->syms[i], env_syms_len);
        n->vals[i] = lval_copy(env->vals[i]);
    }
    return n;
}

void lenv_def(lenv* env, const lval* key, const lval* val) {
    assert(NULL != env);
    assert(NULL != key);
    assert(NULL != val);

    // Iterate until e has no parent
    while (env->parent) {
        env = env->parent;
    }
    // Put value in e
    lenv_put(env, key, val);
}
