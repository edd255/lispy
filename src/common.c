#include "common.h"

#include "values.h"

//=== CONSTRUCTORS =============================================================

/* Create a new number type lval_t */
lval_t* lval_num(long x) {
    lval_t* v = malloc(sizeof(lval_t));
    log_debug("Value (x): %ld. Address: %p", x, v);
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

lval_t* lval_dec(double x) {
    lval_t* v = malloc(sizeof(lval_t));
    log_debug("Value (x): %d. Address: %p", x, v);
    v->type = LVAL_DEC;
    v->dec = x;
    return v;
}

lval_t* lval_err(char* fmt, ...) {
    lval_t* v = malloc(sizeof(lval_t));
    log_debug("Address: %p", v);
    v->type = LVAL_ERR;

    // Create a va list and initialize it
    va_list va;
    va_start(va, fmt);

    // Allocate 512 bytes of space
    v->err = malloc(512);
    log_debug("Address (v->err): %p", v->err);

    // printf the error string with a maximum of 511 character
    vsnprintf(v->err, 511, fmt, va);

    // Reallocate to number of bytes actually used
    v->err = realloc(v->err, strlen(v->err) + 1);
    log_debug("Address (v->err): %p", v->err);

    // Cleanup our va list
    va_end(va);

    return v;
}

lval_t* lval_sym(char* s) {
    assert(s != NULL);
    lval_t* v = malloc(sizeof(lval_t));
    log_debug("Address (v): %p", v->sym);
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    log_debug("Address(v->sym): %p", v->sym);
    strcpy(v->sym, s);
    return v;
}

lval_t* lval_sexpr(void) {
    lval_t* v = malloc(sizeof(lval_t));
    log_debug("Address (v): %p", v);
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

lval_t* lval_qexpr(void) {
    lval_t* v = malloc(sizeof(lval_t));
    log_debug("Address (v): %p", v);
    v->type = LVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

lval_t* lval_fn(lbuiltin_t fn) {
    assert(fn != NULL);
    lval_t* v = malloc(sizeof(lval_t));
    log_debug("Address (v): %p", v);
    v->type = LVAL_FN;
    v->builtin = fn;
    return v;
}

lval_t* lval_lambda(lval_t* formals, lval_t* body) {
    assert(formals != NULL);
    assert(body != NULL);

    lval_t* v = malloc(sizeof(lval_t));
    log_debug("Address (v): %p", v);
    v->type = LVAL_FN;

    // Set builtin to NULL
    v->builtin = NULL;

    // Build new environment
    v->env = lenv_new();

    // Set formals and body
    v->formals = formals;
    v->body = body;
    return v;
}

lval_t* lval_str(const char* s) {
    assert(s != NULL);

    lval_t* v = malloc(sizeof(lval_t));
    log_debug("Address (v): %p", v);
    v->type = LVAL_STR;
    v->str = malloc(strlen(s) + 1);
    log_debug("Address (v->str): %p", v->str);
    strcpy(v->str, s);
    return v;
}

void lval_del(lval_t* v) {
    log_debug("Freeing address: %p", v);
    assert(v != NULL);

    if (v == NULL) {
        return;
    }
    switch (v->type) {
        // Do nothing special for number type
        case LVAL_NUM: {
            break;
        }
        // For Errors or Symbols free the string data
        case LVAL_ERR: {
            if (v->err == NULL) {
                return;
            }
            free(v->err);
            break;
        }
        case LVAL_SYM: {
            if (v->sym == NULL) {
                return;
            }
            free(v->sym);
            break;
        }
        case LVAL_FN: {
            if (!(v->builtin)) {
                lenv_del(v->env);
                lval_del(v->formals);
                lval_del(v->body);
            }
            break;
        }
        case LVAL_STR: {
            if (v->str == NULL) {
                return;
            }
            free(v->str);
            break;
        }
        // If S-Expression or Q-Expression, then delete all elements inside
        case LVAL_QEXPR:
        case LVAL_SEXPR: {
            for (int i = 0; i < v->count; i++) {
                lval_del(v->cell[i]);
            }
            free(v->cell);
            break;
        }
    }
    free(v);
}

//=== ENVIRONMENT ==============================================================

lenv_t* lenv_new(void) {
    lenv_t* e = malloc(sizeof(lenv_t));
    log_debug("Address (e): %p", e);
    e->parent = NULL;
    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;
    return e;
}

void lenv_del(lenv_t* e) {
    assert(e != NULL);
    log_debug("Freeing address: %p", e);

    for (int i = 0; i < e->count; i++) {
        free(e->syms[i]);
        lval_del(e->vals[i]);
    }
    free(e->syms);
    free(e->vals);
    free(e);
}

lval_t* lenv_get(lenv_t* e, lval_t* k) {
    assert(e != NULL);
    assert(k != NULL);

    // Iterate over all items in environment
    for (int i = 0; i < e->count; i++) {
        // Check if the stored string matches the symbol string. If it does,
        // return a copy of the value
        if (strcmp(e->syms[i], k->sym) == 0) {
            return lval_copy(e->vals[i]);
        }
    }
    // If no symbol check in parent otherwise error
    if (e->parent) {
        return lenv_get(e->parent, k);
    } else {
        return lval_err("Unbound symbol '%s", k->sym);
    }
}

void lenv_put(lenv_t* e, const lval_t* k, lval_t* v) {
    assert(e != NULL);
    assert(k != NULL);
    assert(v != NULL);

    // Iterate over all items in environment. This is to see if variable already
    // exists.
    for (int i = 0; i < e->count; i++) {
        // If variable is found delete item at that position. And replace with
        // variable supplied by user.
        if (strcmp(e->syms[i], k->sym) == 0) {
            lval_del(e->vals[i]);
            e->vals[i] = lval_copy(v);
            return;
        }
    }
    // If no existing entry found allocate space for new entry
    e->count++;
    e->vals = realloc(e->vals, sizeof(lval_t*) * e->count);
    log_debug("Address (e->vals): %p", e->vals);

    e->syms = realloc(e->syms, sizeof(char*) * e->count);
    log_debug("Address (e->syms): %p", e->syms);

    // Copy contents of lval and symbol string into new location
    e->vals[e->count - 1] = lval_copy(v);
    e->syms[e->count - 1] = malloc(strlen(k->sym) + 1);
    log_debug("Address (e->syms[e->count - 1]): %p", e->syms[e->count - 1]);

    strcpy(e->syms[e->count - 1], k->sym);
}

lenv_t* lenv_copy(lenv_t* e) {
    assert(e != NULL);

    lenv_t* n = malloc(sizeof(lenv_t));
    log_debug("Address (n): %p", n);
    n->parent = e->parent;
    n->count = e->count;
    n->syms = malloc(sizeof(char*) * (n->count));
    log_debug("Address (n->syms): %p", n->syms);
    n->vals = malloc(sizeof(lval_t*) * (n->count));
    log_debug("n->vals: %p", n->vals);
    log_debug("Address (n->vals): %p", n->vals);
    for (int i = 0; i < e->count; i++) {
        n->syms[i] = malloc(strlen(e->syms[i]) + 1);
        log_debug("Address (n->vals[%d]): %p", i, n->vals[i]);
        strcpy(n->syms[i], e->syms[i]);
        n->vals[i] = lval_copy(e->vals[i]);
    }
    return n;
}

void lenv_def(lenv_t* e, const lval_t* k, lval_t* v) {
    assert(e != NULL);
    assert(k != NULL);
    assert(v != NULL);

    // Iterate until e has no parent
    while (e->parent) {
        e = e->parent;
    }
    // Put value in e
    lenv_put(e, k, v);
}
