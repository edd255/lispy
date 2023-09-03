#include <stdarg.h>

#include "builtins.h"
#include "common.h"
#include "io.h"

//=== CONSTRUCTORS =============================================================

#define LVAL_INIT() lval_init(__func__, __FILE__, __LINE__)

lval_t* lval_init(const char* fn, const char* file, int line) {
#ifdef LOG_ALLOCS
    UNUSED(fn);
    UNUSED(file);
    UNUSED(line);
    lval_t* self = malloc(sizeof(lval_t));
#else
    lval_t* self = log_malloc(sizeof(lval_t), fn, file, line);
#endif
    self->type = 0;
    self->num = 0;
    self->dec = 0.0f;
    self->err = NULL;
    self->sym = NULL;
    self->str = NULL;
    self->builtin = NULL;
    self->env = NULL;
    self->formals = NULL;
    self->body = NULL;
    self->count = 0;
    self->cell = NULL;
    return self;
}

/* Create a new number type lval_t */
lval_t* lval_num(long value) {
    lval_t* self = LVAL_INIT();
    self->type = LISPY_VAL_NUM;
    self->num = value;
    return self;
}

lval_t* lval_dec(double value) {
    lval_t* self = LVAL_INIT();
    self->type = LISPY_VAL_DEC;
    self->dec = value;
    return self;
}

lval_t* lval_err(char* fmt, ...) {
    lval_t* self = LVAL_INIT();
    self->type = LISPY_VAL_ERR;

    // Create a va list and initialize it
    va_list arg_list;
    va_start(arg_list, fmt);

    // Allocate 512 bytes of space
    self->err = MALLOC(BUFSIZE);

    // printf the error string with a maximum of 511 character
    vsnprintf(self->err, BUFSIZE - 1, fmt, arg_list);

    // Reallocate to number of bytes actually used
    size_t str_len = strlen(self->err) + 1;
    self->err = REALLOC(self->err, str_len);

    // Cleanup our va list
    va_end(arg_list);
    return self;
}

lval_t* lval_sym(char* str) {
    assert(NULL != str);
    lval_t* self = LVAL_INIT();
    self->type = LISPY_VAL_SYM;
    size_t len = strlen(str) + 1;
    self->sym = MALLOC(len);
    strlcpy(self->sym, str, len);
    return self;
}

lval_t* lval_str(const char* str) {
    assert(NULL != str);
    lval_t* self = LVAL_INIT();
    self->type = LISPY_VAL_STR;
    size_t len = strlen(str) + 1;
    self->str = MALLOC(len);
    strlcpy(self->str, str, len);
    return self;
}

lval_t* lval_sexpr(void) {
    lval_t* self = LVAL_INIT();
    self->type = LISPY_VAL_SEXPR;
    self->count = 0;
    self->cell = NULL;
    return self;
}

lval_t* lval_qexpr(void) {
    lval_t* self = LVAL_INIT();
    self->type = LISPY_VAL_QEXPR;
    self->count = 0;
    self->cell = NULL;
    return self;
}

lval_t* lval_fn(lbuiltin_t fn) {
    assert(NULL != fn);
    lval_t* self = LVAL_INIT();
    self->type = LISPY_VAL_FN;
    self->builtin = fn;
    return self;
}

lval_t* lval_lambda(lval_t* formals, lval_t* body) {
    assert(NULL != formals);
    assert(NULL != body);

    lval_t* self = LVAL_INIT();
    self->type = LISPY_VAL_FN;

    // Set builtin to NULL
    self->builtin = NULL;

    // Build new environment
    self->env = lenv_new();

    // Set formals and body
    self->formals = formals;
    self->body = body;
    return self;
}

//=== METHODS ==================================================================

lval_t* lval_copy(const lval_t* self) {
    assert(NULL != self);

    lval_t* x = MALLOC(sizeof(lval_t));
    x->type = self->type;
    switch (self->type) {
        // Copy Functions and Numbers Directly
        case LISPY_VAL_FN: {
            if (self->builtin) {
                x->builtin = self->builtin;
            } else {
                x->builtin = NULL;
                x->env = lenv_copy(self->env);
                x->formals = lval_copy(self->formals);
                x->body = lval_copy(self->body);
            }
            break;
        }
        case LISPY_VAL_NUM: {
            x->num = self->num;
            break;
        }
        case LISPY_VAL_DEC: {
            x->dec = self->dec;
            break;
        }
        // Copy Strings using malloc and strlcpy
        case LISPY_VAL_ERR: {
            size_t err_len = strlen(self->err) + 1;
            x->err = MALLOC(err_len);
            strlcpy(x->err, self->err, err_len);
            break;
        }
        case LISPY_VAL_SYM: {
            size_t sym_len = strlen(self->sym) + 1;
            x->sym = MALLOC(sym_len);
            strlcpy(x->sym, self->sym, sym_len);
            break;
        }
        case LISPY_VAL_STR: {
            size_t str_len = strlen(self->str) + 1;
            x->str = MALLOC(str_len);
            strlcpy(x->str, self->str, str_len);
            break;
        }
        // Copy Lists by copying each sub-expression
        case LISPY_VAL_SEXPR:
        case LISPY_VAL_QEXPR:
            x->count = self->count;
            x->cell = MALLOC(sizeof(lval_t*) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = lval_copy(self->cell[i]);
            }
            break;
    }
    return x;
}

lval_t* lval_add(lval_t* self, lval_t* other) {
    assert(NULL != self);
    assert(NULL != other);

    self->count++;
    self->cell = REALLOC(self->cell, sizeof(lval_t*) * self->count);
    self->cell[self->count - 1] = other;
    return self;
}

lval_t* lval_join(lval_t* self, lval_t* other) {
    assert(NULL != self);
    assert(NULL != other);
    assert(NULL != other->cell);
    LCHECK(
        self,
        (self->type == LISPY_VAL_QEXPR || self->type == LISPY_VAL_STR),
        "%s expected quoted expression or string but got %s",
        __func__,
        ltype_name(self->type)
    );
    switch (self->type) {
        case LISPY_VAL_QEXPR: {
            LCHECK(
                self,
                other->type == LISPY_VAL_QEXPR,
                "%s expected quoted expression but got %s",
                __func__,
                ltype_name(other->type)
            );
            break;
        }
        case LISPY_VAL_STR: {
            LCHECK(
                self,
                other->type == LISPY_VAL_STR,
                "%s expected string but got %s",
                __func__,
                ltype_name(other->type)
            );
            break;
        }
    }
    // For strings
    if ((self->type == LISPY_VAL_STR) && (other->type == LISPY_VAL_STR)) {
        char str[BUFSIZE];
        strlcpy(str, self->str, BUFSIZE);
        strlcat(str, other->str, BUFSIZE);
        lval_del(self);
        lval_del(other);
        return lval_str(str);
    }
    // For each cell in 'other' add it to 'self'
    for (int i = 0; i < other->count; i++) {
        assert(NULL != other->cell[i]);
        self = lval_add(self, other->cell[i]);
    }
    // Delete the empty 'other' and return 'self'
    FREE(other->cell);
    FREE(other);
    return self;
}

lval_t* lval_pop(lval_t* self, const int idx) {
    assert(NULL != self);
    LCHECK(
        self,
        self->count > idx,
        "%s passed %d as index but list has a length of %d",
        __func__,
        idx,
        self->count
    );

    // Find the item at "i"
    lval_t* value = self->cell[idx];

    // Shift memory after the item at "i" over the top
    memmove(
        &self->cell[idx],
        &self->cell[idx + 1],
        sizeof(lval_t*) * (self->count - idx - 1)
    );

    // Decrease the count of items in the list
    self->count--;

    // Reallocate the memory used
    self->cell = REALLOC(self->cell, sizeof(lval_t*) * self->count);
    return value;
}

lval_t* lval_take(lval_t* self, const int idx) {
    assert(NULL != self);

    lval_t* value = lval_pop(self, idx);
    lval_del(self);
    return value;
}

lval_t* lval_call(lenv_t* env, lval_t* fn, lval_t* args) {
    assert(NULL != env);
    assert(NULL != fn);
    assert(NULL != args);

    // If Builtin, then simply call that
    if (fn->builtin) {
        return fn->builtin(env, args);
    }
    // Record argument counts
    int given = args->count;
    int total = fn->formals->count;

    // While arguments still remain to be processed
    while (args->count) {
        // If we've run out of formal arguments to bind
        if (0 == fn->formals->count) {
            lval_del(args);
            return lval_err(
                "Function passed too many arguments. Got %i. Expected %i.",
                given,
                total
            );
        }
        // Pop the first symbol from the formals
        lval_t* sym = lval_pop(fn->formals, 0);

        // Special case to deal with '&'
        if (0 == strcmp(sym->sym, "&")) {
            // Ensure "&" is followed by another symbol
            if (1 != fn->formals->count) {
                lval_del(args);
                return lval_err(
                    "Function format invalid. Symbol '&' not followed by single symbol."
                );
            }

            // Next formal should be bound to remaining arguments
            lval_t* nsym = lval_pop(fn->formals, 0);
            lenv_put(fn->env, nsym, builtin_list(env, args));
            lval_del(sym);
            lval_del(nsym);
            break;
        }

        // Pop the next argument from the list
        lval_t* val = lval_pop(args, 0);

        // Bind a copy into the fn's environment
        lenv_put(fn->env, sym, val);

        // Delete symbol and value
        lval_del(sym);
        lval_del(val);
    }
    // Argument list is now bound so can be cleaned up
    lval_del(args);

    if (0 < fn->formals->count && 0 == strcmp(fn->formals->cell[0]->sym, "&")) {
        // Check to ensure that & is not passed invalidly
        if (2 != fn->formals->count) {
            return lval_err(
                "Function format invalid. Symbol '&' not followed by single symbol."
            );
        }
        // Pop and delete '&' symbol
        lval_t* popped = lval_pop(fn->formals, 0);
        lval_del(popped);

        // Pop next symbol and create empty list
        lval_t* sym = lval_pop(fn->formals, 0);
        lval_t* val = lval_qexpr();

        // Bind to environment and delete
        lenv_put(fn->env, sym, val);
        lval_del(sym);
        lval_del(val);
    }

    // If all formals have been bound evaluate
    if (0 == fn->formals->count) {
        // Set environment parent to evaluation environment
        fn->env->parent = env;

        // Evaluate and return
        lval_t* new_sexpr = lval_sexpr();
        lval_t* body_copy = lval_copy(fn->body);

        assert(NULL != new_sexpr);
        assert(NULL != body_copy);
        assert(NULL != fn->env);

        return builtin_eval(fn->env, lval_add(new_sexpr, body_copy));
    }
    // Otherwise return partially evaluated function
    return lval_copy(fn);
}

int lval_eq(const lval_t* self, const lval_t* other) {
    assert(NULL != self);
    assert(NULL != other);

    // Different types are always unequal
    if (self->type != other->type) {
        return false;
    }
    // Compare based upon type
    switch (self->type) {
        // Compare number value
        case LISPY_VAL_NUM: {
            return (self->num == other->num);
        }
        case LISPY_VAL_DEC: {
            return (self->dec == other->dec);
        }
        // Compare string values
        case LISPY_VAL_ERR: {
            return 0 == strcmp(self->err, other->err);
        }
        case LISPY_VAL_SYM: {
            return 0 == strcmp(self->sym, other->sym);
        }
        // If builtin compare, otherwise compare formals and body
        case LISPY_VAL_FN: {
            if (self->builtin || other->builtin) {
                return self->builtin == other->builtin;
            }
            return lval_eq(self->formals, other->formals)
                && lval_eq(self->body, other->body);
        }
        case LISPY_VAL_STR: {
            return 0 == strcmp(self->str, other->str);
        }
        // If list compare every individual element
        case LISPY_VAL_QEXPR:
        case LISPY_VAL_SEXPR: {
            if (self->count != other->count) {
                return 0;
            }
            for (int i = 0; i < self->count; i++) {
                // If any element not equal then whole list not equal
                if (!lval_eq(self->cell[i], other->cell[i])) {
                    return false;
                }
            }
            // Otherwise lists must be equal;
            return true;
        }
    }
    return false;
}

void lval_del(lval_t* self) {
    assert(NULL != self);
    if (NULL == self) {
        return;
    }
    switch (self->type) {
        // Do nothing special for number type
        case LISPY_VAL_NUM: {
            break;
        }
        // For Errors or Symbols free the string data
        case LISPY_VAL_ERR: {
            if (NULL == self->err) {
                break;
            }
            FREE(self->err);
            break;
        }
        case LISPY_VAL_SYM: {
            if (NULL == self->sym) {
                break;
            }
            FREE(self->sym);
            break;
        }
        case LISPY_VAL_FN: {
            if (!(self->builtin)) {
                lenv_del(self->env);
                lval_del(self->formals);
                lval_del(self->body);
            }
            break;
        }
        case LISPY_VAL_STR: {
            if (NULL == self->str) {
                break;
            }
            FREE(self->str);
            break;
        }
        // If S-Expression or Q-Expression, then delete all elements inside
        case LISPY_VAL_QEXPR:
        case LISPY_VAL_SEXPR: {
            for (int i = 0; i < self->count; i++) {
                lval_del(self->cell[i]);
            }
            FREE(self->cell);
            break;
        }
    }
    FREE(self);
}
