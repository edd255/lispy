#include "common.h"

#include "values.h"

//=== CONSTRUCTORS =============================================================

/* Create a new number type lval_t */
lval_t* lval_num(long x) {
    lval_t* val = MALLOC(sizeof(lval_t));
    val->type = LVAL_NUM;
    val->num = x;
    return val;
}

lval_t* lval_dec(double x) {
    lval_t* val = MALLOC(sizeof(lval_t));
    val->type = LVAL_DEC;
    val->dec = x;
    return val;
}

lval_t* lval_err(char* fmt, ...) {
    lval_t* val = MALLOC(sizeof(lval_t));
    val->type = LVAL_ERR;

    // Create a va list and initialize it
    va_list arg_list;
    va_start(arg_list, fmt);

    // Allocate 512 bytes of space
    val->err = MALLOC(512);

    // printf the error string with a maximum of 511 character
    vsnprintf(val->err, 511, fmt, arg_list);

    // Reallocate to number of bytes actually used
    val->err = REALLOC(val->err, strlen(val->err) + 1);

    // Cleanup our va list
    va_end(arg_list);

    return val;
}

lval_t* lval_sym(char* str) {
    assert(NULL != str);
    lval_t* val = MALLOC(sizeof(lval_t));
    val->type = LVAL_SYM;
    val->sym = MALLOC(strlen(str) + 1);
    strcpy(val->sym, str);
    return val;
}

lval_t* lval_sexpr(void) {
    lval_t* val = MALLOC(sizeof(lval_t));
    val->type = LVAL_SEXPR;
    val->count = 0;
    val->cell = NULL;
    return val;
}

lval_t* lval_qexpr(void) {
    lval_t* val = MALLOC(sizeof(lval_t));
    val->type = LVAL_QEXPR;
    val->count = 0;
    val->cell = NULL;
    return val;
}

lval_t* lval_fn(lbuiltin_t fn) {
    assert(NULL != fn);
    lval_t* val = MALLOC(sizeof(lval_t));
    val->type = LVAL_FN;
    val->builtin = fn;
    return val;
}

lval_t* lval_lambda(lval_t* formals, lval_t* body) {
    assert(NULL != formals);
    assert(NULL != body);

    lval_t* val = MALLOC(sizeof(lval_t));
    val->type = LVAL_FN;

    // Set builtin to NULL
    val->builtin = NULL;

    // Build new environment
    val->env = lenv_new();

    // Set formals and body
    val->formals = formals;
    val->body = body;
    return val;
}

lval_t* lval_str(const char* str) {
    assert(NULL != str);
    lval_t* val = MALLOC(sizeof(lval_t));
    val->type = LVAL_STR;
    val->str = MALLOC(strlen(str) + 1);
    strcpy(val->str, str);
    return val;
}

void lval_del(lval_t* val) {
    assert(NULL != val);
    if (NULL == val) {
        return;
    }
    switch (val->type) {
        // Do nothing special for number type
        case LVAL_NUM: {
            break;
        }
        // For Errors or Symbols free the string data
        case LVAL_ERR: {
            if (NULL == val->err) {
                break;
            }
            FREE(val->err);
            break;
        }
        case LVAL_SYM: {
            if (NULL == val->sym) {
                break;
            }
            FREE(val->sym);
            break;
        }
        case LVAL_FN: {
            if (!(val->builtin)) {
                lenv_del(val->env);
                lval_del(val->formals);
                lval_del(val->body);
            }
            break;
        }
        case LVAL_STR: {
            if (NULL == val->str) {
                break;
            }
            FREE(val->str);
            break;
        }
        // If S-Expression or Q-Expression, then delete all elements inside
        case LVAL_QEXPR:
        case LVAL_SEXPR: {
            for (int i = 0; i < val->count; i++) {
                lval_del(val->cell[i]);
            }
            FREE(val->cell);
            break;
        }
    }
    FREE(val);
}

//=== ENVIRONMENT ==============================================================

lenv_t* lenv_new(void) {
    lenv_t* env = MALLOC(sizeof(lenv_t));
    env->parent = NULL;
    env->count = 0;
    env->syms = NULL;
    env->vals = NULL;
    return env;
}

void lenv_del(lenv_t* env) {
    assert(NULL != env);

    for (int i = 0; i < env->count; i++) {
        FREE(env->syms[i]);
        lval_del(env->vals[i]);
    }
    FREE(env->syms);
    FREE(env->vals);
    FREE(env);
}

lval_t* lenv_get(lenv_t* env, lval_t* key) {
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

void lenv_put(lenv_t* env, const lval_t* key, const lval_t* val) {
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
    env->vals = REALLOC(env->vals, sizeof(lval_t*) * env->count);
    env->syms = REALLOC(env->syms, sizeof(char*) * env->count);

    // Copy contents of lval and symbol string into new location
    env->vals[env->count - 1] = lval_copy(val);
    env->syms[env->count - 1] = MALLOC(strlen(key->sym) + 1);

    strcpy(env->syms[env->count - 1], key->sym);
}

lenv_t* lenv_copy(lenv_t* env) {
    assert(NULL != env);

    lenv_t* n = MALLOC(sizeof(lenv_t));
    n->parent = env->parent;
    n->count = env->count;
    n->syms = MALLOC(sizeof(char*) * (n->count));
    n->vals = MALLOC(sizeof(lval_t*) * (n->count));
    for (int i = 0; i < env->count; i++) {
        n->syms[i] = MALLOC(strlen(env->syms[i]) + 1);
        strcpy(n->syms[i], env->syms[i]);
        n->vals[i] = lval_copy(env->vals[i]);
    }
    return n;
}

void lenv_def(lenv_t* env, const lval_t* key, lval_t* val) {
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

//=== LOGGING MEMORY ALLOCATIONS ===============================================
void* log_malloc(size_t size, const char* fn, const char* file, int line) {
    void* ptr = malloc(size);
    log_log(LOGC_TRACE, fn, file, line, "Address: %p", ptr);
    return ptr;
}

void* log_realloc(
    void* old_ptr,
    size_t size,
    const char* fn,
    const char* file,
    int line
) {
    void* new_ptr = realloc(old_ptr, size);
    log_log(
        LOGC_TRACE,
        fn,
        file,
        line,
        "Old address: %p. New Address: %p",
        old_ptr,
        new_ptr
    );
    return new_ptr;
}

void log_free(void* ptr, const char* fn, const char* file, int line) {
    log_log(LOGC_TRACE, fn, file, line, "Freeing: %p", ptr);
    free(ptr);
}
