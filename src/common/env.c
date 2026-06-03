#include "common.h"

//=== ENVIRONMENT ==============================================================
Environment* env_new(void) {
    Environment* env = malloc(sizeof(Environment));
    env->parent = NULL;
    env->count = 0;
    env->syms = NULL;
    env->vals = NULL;
    return env;
}

void env_del(Environment* env) {
    ASSERT(env != NULL);
    for (int i = 0; i < env->count; i++) {
        free(env->syms[i]);
        val_del(env->vals[i]);
    }
    free(env->syms);
    free(env->vals);
    free(env);
}

Value* env_get(Environment* env, Value* key) {
    ASSERT(env != NULL);
    ASSERT(key != NULL);
    // Iterate over all items in environment
    for (int i = 0; i < env->count; i++) {
        // Check if the stored string matches the symbol string. If it does,
        // return a copy of the value
        if (strcmp(env->syms[i], key->sym) == 0) {
            return val_copy(env->vals[i]);
        }
    }
    // If no symbol check in parent otherwise error
    if (env->parent) {
        return env_get(env->parent, key);
    }
    return val_err("Unbound symbol '%s'", key->sym);
}

void env_put(Environment* env, const Value* key, const Value* val) {
    ASSERT(env != NULL);
    ASSERT(key != NULL);
    ASSERT(val != NULL);
    // Iterate over all items in environment. This is to see if variable already
    // exists.
    for (int i = 0; i < env->count; i++) {
        // If variable is found delete item at that position. And replace with
        // variable supplied by user.
        if (strcmp(env->syms[i], key->sym) == 0) {
            val_del(env->vals[i]);
            env->vals[i] = val_copy(val);
            return;
        }
    }
    // If no existing entry found allocate space for new entry
    env->count++;
    env->vals = realloc(env->vals, sizeof(Value*) * env->count);
    env->syms = realloc(env->syms, sizeof(char*) * env->count);
    // Copy contents of Value and symbol string into new location
    env->vals[env->count - 1] = val_copy(val);
    env->syms[env->count - 1] = malloc(key->len + 1);
    strlcpy(env->syms[env->count - 1], key->sym, key->len + 1);
}

Environment* env_copy(Environment* env) {
    ASSERT(env != NULL);
    Environment* n = malloc(sizeof(Environment));
    n->parent = env->parent;
    n->count = env->count;
    n->syms = malloc(sizeof(char*) * (n->count));
    n->vals = malloc(sizeof(Value*) * (n->count));
    for (int i = 0; i < env->count; i++) {
        size_t env_syms_len = strlen(env->syms[i]) + 1;
        n->syms[i] = malloc(env_syms_len);
        strlcpy(n->syms[i], env->syms[i], env_syms_len);
        n->vals[i] = val_copy(env->vals[i]);
    }
    return n;
}

void env_def(Environment* env, const Value* key, const Value* val) {
    ASSERT(env != NULL);
    ASSERT(key != NULL);
    ASSERT(val != NULL);
    // Iterate until e has no parent
    while (env->parent) {
        env = env->parent;
    }
    // Put value in e
    env_put(env, key, val);
}
