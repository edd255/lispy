#include <stdarg.h>

#include "builtins.h"
#include "common.h"
#include "io.h"

//=== CONSTRUCTORS =============================================================

#define LVAL_INIT() val_init(__func__, __FILE__, __LINE__)

Value* val_init(const char* fn, const char* file, int line) {
#ifdef LOG_ALLOCS
    Value* self = log_malloc(sizeof(Value), fn, file, line);
#else
    UNUSED(fn);
    UNUSED(file);
    UNUSED(line);
    Value* self = malloc(sizeof(Value));
#endif
    self->type = 0;
    self->num = 0;
    self->dec = 0.0f;
    self->err = NULL;
    self->sym = NULL;
    self->str = NULL;
    self->len = 0;
    self->builtin = NULL;
    self->env = NULL;
    self->formals = NULL;
    self->body = NULL;
    self->count = 0;
    self->cell = NULL;
    return self;
}

/* Create a new number type Value */
Value* val_num(long value) {
    Value* self = LVAL_INIT();
    self->type = LISPY_VAL_NUM;
    self->num = value;
    return self;
}

Value* val_dec(double value) {
    Value* self = LVAL_INIT();
    self->type = LISPY_VAL_DEC;
    self->dec = value;
    return self;
}

Value* val_err(char* fmt, ...) {
    Value* self = LVAL_INIT();
    self->type = LISPY_VAL_ERR;
    // Create a va list and initialize it
    va_list arg_list;
    va_start(arg_list, fmt);
    // Allocate 512 bytes of space
    self->err = MALLOC(BUFSIZE);
    // printf the error string with a maximum of 511 character
    vsnprintf(self->err, BUFSIZE - 1, fmt, arg_list);
    // Reallocate to number of bytes actually used
    self->len = strlen(self->err);
    self->err = REALLOC(self->err, self->len + 1);
    // Cleanup our va list
    va_end(arg_list);
    return self;
}

Value* val_sym(char* str) {
    assert(NULL != str);
    Value* self = LVAL_INIT();
    self->type = LISPY_VAL_SYM;
    self->len = strlen(str);
    self->sym = MALLOC(self->len + 1);
    strlcpy(self->sym, str, self->len + 1);
    return self;
}

Value* val_str(const char* str) {
    assert(NULL != str);
    Value* self = LVAL_INIT();
    self->type = LISPY_VAL_STR;
    self->len = strlen(str);
    self->str = MALLOC(self->len + 1);
    strlcpy(self->str, str, self->len + 1);
    return self;
}

Value* val_sexpr(void) {
    Value* self = LVAL_INIT();
    self->type = LISPY_VAL_SEXPR;
    self->count = 0;
    self->cell = NULL;
    return self;
}

Value* val_qexpr(void) {
    Value* self = LVAL_INIT();
    self->type = LISPY_VAL_QEXPR;
    self->count = 0;
    self->cell = NULL;
    return self;
}

Value* val_fn(Function fn) {
    assert(NULL != fn);
    Value* self = LVAL_INIT();
    self->type = LISPY_VAL_FN;
    self->builtin = fn;
    return self;
}

Value* val_lambda(Value* formals, Value* body) {
    assert(NULL != formals);
    assert(NULL != body);
    Value* self = LVAL_INIT();
    self->type = LISPY_VAL_FN;
    // Set builtin to NULL
    self->builtin = NULL;
    // Build new environment
    self->env = env_new();
    // Set formals and body
    self->formals = formals;
    self->body = body;
    return self;
}

//=== METHODS ==================================================================

Value* val_copy(const Value* self) {
    assert(NULL != self);
    Value* x = MALLOC(sizeof(Value));
    x->type = self->type;
    switch (self->type) {
        // Copy Functions and Numbers directly
        case LISPY_VAL_FN: {
            if (self->builtin) {
                x->builtin = self->builtin;
            } else {
                x->builtin = NULL;
                x->env = env_copy(self->env);
                x->formals = val_copy(self->formals);
                x->body = val_copy(self->body);
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
            x->err = MALLOC(self->len + 1);
            strlcpy(x->err, self->err, self->len + 1);
            x->len = self->len;
            break;
        }
        case LISPY_VAL_SYM: {
            x->sym = MALLOC(self->len + 1);
            strlcpy(x->sym, self->sym, self->len + 1);
            x->len = self->len;
            break;
        }
        case LISPY_VAL_STR: {
            x->str = MALLOC(self->len + 1);
            strlcpy(x->str, self->str, self->len + 1);
            x->len = self->len;
            break;
        }
        // Copy Lists by copying each sub-expression
        case LISPY_VAL_SEXPR:
        case LISPY_VAL_QEXPR:
            x->count = self->count;
            x->cell = MALLOC(sizeof(Value*) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = val_copy(self->cell[i]);
            }
            break;
    }
    return x;
}

Value* val_add(Value* self, Value* other) {
    assert(NULL != self);
    assert(NULL != other);
    self->count++;
    self->cell = REALLOC(self->cell, sizeof(Value*) * self->count);
    self->cell[self->count - 1] = other;
    return self;
}

Value* val_join(Value* self, Value* other) {
    assert(NULL != self);
    assert(NULL != other);
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
        val_del(self);
        val_del(other);
        return val_str(str);
    }
    // For each cell in 'other' add it to 'self'
    for (int i = 0; i < other->count; i++) {
        assert(NULL != other->cell[i]);
        self = val_add(self, other->cell[i]);
    }
    // Delete the empty 'other' and return 'self'
    FREE(other->cell);
    FREE(other);
    return self;
}

Value* val_pop(Value* self, const int idx) {
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
    Value* value = self->cell[idx];
    // Shift memory after the item at "i" over the top
    memmove(
        &self->cell[idx],
        &self->cell[idx + 1],
        sizeof(Value*) * (self->count - idx - 1)
    );
    // Decrease the count of items in the list
    self->count--;
    // Reallocate the memory used
    self->cell = REALLOC(self->cell, sizeof(Value*) * self->count);
    return value;
}

Value* val_take(Value* self, const int idx) {
    assert(NULL != self);
    Value* value = val_pop(self, idx);
    val_del(self);
    return value;
}

Value* val_call(Environment* env, Value* fn, Value* args) {
    assert(NULL != env);
    assert(NULL != fn);
    assert(NULL != args);
    // If Function, then simply call that
    if (NULL != fn->builtin) {
        return fn->builtin(env, args);
    }
    // Record argument counts
    int given = args->count;
    int total = fn->formals->count;
    // While arguments still remain to be processed
    while (args->count) {
        // If we've run out of formal arguments to bind
        if (0 == fn->formals->count) {
            val_del(args);
            return val_err(
                "Function passed too many arguments. Got %i. Expected %i.",
                given,
                total
            );
        }
        // Pop the first symbol from the formals
        Value* sym = val_pop(fn->formals, 0);
        // Special case to deal with '&'
        if (0 == strcmp(sym->sym, "&")) {
            // Ensure "&" is followed by another symbol
            if (1 != fn->formals->count) {
                val_del(args);
                return val_err(
                    "Function format invalid. Symbol '&' not followed by single symbol."
                );
            }
            // Next formal should be bound to remaining arguments
            Value* nsym = val_pop(fn->formals, 0);
            env_put(fn->env, nsym, builtin_list(env, args));
            val_del(sym);
            val_del(nsym);
            break;
        }
        // Pop the next argument from the list
        Value* val = val_pop(args, 0);
        // Bind a copy into the fn's environment
        env_put(fn->env, sym, val);
        // Delete symbol and value
        val_del(sym);
        val_del(val);
    }
    // Argument list is now bound so can be cleaned up
    val_del(args);
    if (0 < fn->formals->count && 0 == strcmp(fn->formals->cell[0]->sym, "&")) {
        // Check to ensure that & is not passed invalidly
        if (2 != fn->formals->count) {
            return val_err(
                "Function format invalid. Symbol '&' not followed by single symbol."
            );
        }
        // Pop and delete '&' symbol
        Value* popped = val_pop(fn->formals, 0);
        val_del(popped);
        // Pop next symbol and create empty list
        Value* sym = val_pop(fn->formals, 0);
        Value* val = val_qexpr();
        // Bind to environment and delete
        env_put(fn->env, sym, val);
        val_del(sym);
        val_del(val);
    }
    // If all formals have been bound evaluate
    if (0 == fn->formals->count) {
        // Set environment parent to evaluation environment
        fn->env->parent = env;
        // Evaluate and return
        Value* new_sexpr = val_sexpr();
        Value* body_copy = val_copy(fn->body);
        assert(NULL != new_sexpr);
        assert(NULL != body_copy);
        assert(NULL != fn->env);
        return builtin_eval(fn->env, val_add(new_sexpr, body_copy));
    }
    // Otherwise return partially evaluated function
    return val_copy(fn);
}

int val_eq(const Value* self, const Value* other) {
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
            return val_eq(self->formals, other->formals)
                && val_eq(self->body, other->body);
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
                if (!val_eq(self->cell[i], other->cell[i])) {
                    return false;
                }
            }
            // Otherwise lists must be equal;
            return true;
        }
    }
    return false;
}

void val_del(Value* self) {
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
                env_del(self->env);
                val_del(self->formals);
                val_del(self->body);
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
                val_del(self->cell[i]);
            }
            FREE(self->cell);
            break;
        }
    }
    FREE(self);
}
