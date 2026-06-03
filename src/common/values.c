#include <stdarg.h>

#include "builtins.h"
#include "common.h"
#include "io.h"

//=== CONSTRUCTORS =============================================================

Value* val_init(void) {
    Value* self = malloc(sizeof(Value));
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
    Value* self = val_init();
    self->type = LISPY_VAL_NUM;
    self->num = value;
    return self;
}

Value* val_dec(double value) {
    Value* self = val_init();
    self->type = LISPY_VAL_DEC;
    self->dec = value;
    return self;
}

Value* val_err(char* fmt, ...) {
    Value* self = val_init();
    self->type = LISPY_VAL_ERR;
    // Create a va list and initialize it
    va_list arg_list;
    va_start(arg_list, fmt);
    // Allocate 512 bytes of space
    self->err = malloc(BUFSIZE);
    // printf the error string with a maximum of 511 character
    vsnprintf(self->err, BUFSIZE - 1, fmt, arg_list);
    // Reallocate to number of bytes actually used
    self->len = strlen(self->err);
    self->err = realloc(self->err, self->len + 1);
    // Cleanup our va list
    va_end(arg_list);
    return self;
}

Value* val_sym(char* str) {
    ASSERT(str != NULL);
    Value* self = val_init();
    self->type = LISPY_VAL_SYM;
    self->len = strlen(str);
    self->sym = malloc(self->len + 1);
    strlcpy(self->sym, str, self->len + 1);
    return self;
}

Value* val_str(const char* str) {
    ASSERT(str != NULL);
    Value* self = val_init();
    self->type = LISPY_VAL_STR;
    self->len = strlen(str);
    self->str = malloc(self->len + 1);
    strlcpy(self->str, str, self->len + 1);
    return self;
}

Value* val_sexpr(void) {
    Value* self = val_init();
    self->type = LISPY_VAL_SEXPR;
    self->count = 0;
    self->cell = NULL;
    return self;
}

Value* val_qexpr(void) {
    Value* self = val_init();
    self->type = LISPY_VAL_QEXPR;
    self->count = 0;
    self->cell = NULL;
    return self;
}

Value* val_fn(Function fn) {
    ASSERT(fn != NULL);
    Value* self = val_init();
    self->type = LISPY_VAL_FN;
    self->builtin = fn;
    return self;
}

Value* val_lambda(Environment* env, Value* formals, Value* body) {
    ASSERT(env != NULL);
    ASSERT(formals != NULL);
    ASSERT(body != NULL);
    Value* self = val_init();
    self->type = LISPY_VAL_FN;
    // Set builtin to NULL
    self->builtin = NULL;
    // Capture local bindings when lambdas are created inside another call.
    self->env = env->parent ? env_copy(env) : env_new();
    // Set formals and body
    self->formals = formals;
    self->body = body;
    return self;
}

//=== METHODS ==================================================================

Value* val_copy(const Value* self) {
    ASSERT(self != NULL);
    Value* x = malloc(sizeof(Value));
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
            x->err = malloc(self->len + 1);
            strlcpy(x->err, self->err, self->len + 1);
            x->len = self->len;
            break;
        }
        case LISPY_VAL_SYM: {
            x->sym = malloc(self->len + 1);
            strlcpy(x->sym, self->sym, self->len + 1);
            x->len = self->len;
            break;
        }
        case LISPY_VAL_STR: {
            x->str = malloc(self->len + 1);
            strlcpy(x->str, self->str, self->len + 1);
            x->len = self->len;
            break;
        }
        // Copy Lists by copying each sub-expression
        case LISPY_VAL_SEXPR:
        case LISPY_VAL_QEXPR:
            x->count = self->count;
            x->cell = malloc(sizeof(Value*) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = val_copy(self->cell[i]);
            }
            break;
    }
    return x;
}

Value* val_add(Value* self, Value* other) {
    ASSERT(self != NULL);
    ASSERT(other != NULL);
    self->count++;
    self->cell = realloc(self->cell, sizeof(Value*) * self->count);
    self->cell[self->count - 1] = other;
    return self;
}

Value* val_join(Value* self, Value* other) {
    ASSERT(self != NULL);
    ASSERT(other != NULL);
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
        size_t joined_len = self->len + other->len + 1;
        char* str = malloc(joined_len);
        strlcpy(str, self->str, joined_len);
        strlcat(str, other->str, joined_len);
        val_del(self);
        val_del(other);
        Value* joined = val_str(str);
        free(str);
        return joined;
    }
    // For each cell in 'other' add it to 'self'
    for (int i = 0; i < other->count; i++) {
        ASSERT(other->cell[i] != NULL);
        self = val_add(self, other->cell[i]);
    }
    // Delete the empty 'other' and return 'self'
    free(other->cell);
    free(other);
    return self;
}

Value* val_pop(Value* self, const int idx) {
    ASSERT(self != NULL);
    if (idx < 0 || self->count <= idx) {
        return val_err(
            "%s passed %d as index but list has a length of %d",
            __func__,
            idx,
            self->count
        );
    }
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
    self->cell = realloc(self->cell, sizeof(Value*) * self->count);
    return value;
}

Value* val_take(Value* self, const int idx) {
    ASSERT(self != NULL);
    Value* value = val_pop(self, idx);
    val_del(self);
    return value;
}

Value* val_call(Environment* env, Value* fn, Value* args) {
    ASSERT(env != NULL);
    ASSERT(fn != NULL);
    ASSERT(args != NULL);
    // If Function, then simply call that
    if (fn->builtin != NULL) {
        return fn->builtin(env, args);
    }
    // Record argument counts
    int given = args->count;
    int total = fn->formals->count;
    // While arguments still remain to be processed
    while (args->count) {
        // If we've run out of formal arguments to bind
        if (fn->formals->count == 0) {
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
        if (strcmp(sym->sym, "&") == 0) {
            // Ensure "&" is followed by another symbol
            if (fn->formals->count != 1) {
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
    if (fn->formals->count > 0 && strcmp(fn->formals->cell[0]->sym, "&") == 0) {
        // Check to ensure that & is not passed invalidly
        if (fn->formals->count != 2) {
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
    if (fn->formals->count == 0) {
        // Set environment parent to evaluation environment
        fn->env->parent = env;
        // Evaluate and return
        Value* new_sexpr = val_sexpr();
        Value* body_copy = val_copy(fn->body);
        ASSERT(new_sexpr != NULL);
        ASSERT(body_copy != NULL);
        ASSERT(fn->env != NULL);
        return builtin_eval(fn->env, val_add(new_sexpr, body_copy));
    }
    // Otherwise return partially evaluated function
    return val_copy(fn);
}

int val_eq(const Value* self, const Value* other) {
    ASSERT(self != NULL);
    ASSERT(other != NULL);
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
            return strcmp(self->err, other->err) == 0;
        }
        case LISPY_VAL_SYM: {
            return strcmp(self->sym, other->sym) == 0;
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
            return strcmp(self->str, other->str) == 0;
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
    ASSERT(self != NULL);
    if (self == NULL) {
        return;
    }
    switch (self->type) {
        // Do nothing special for number type
        case LISPY_VAL_NUM: {
            break;
        }
        // For Errors or Symbols free the string data
        case LISPY_VAL_ERR: {
            if (self->err == NULL) {
                break;
            }
            free(self->err);
            break;
        }
        case LISPY_VAL_SYM: {
            if (self->sym == NULL) {
                break;
            }
            free(self->sym);
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
            if (self->str == NULL) {
                break;
            }
            free(self->str);
            break;
        }
        // If S-Expression or Q-Expression, then delete all elements inside
        case LISPY_VAL_QEXPR:
        case LISPY_VAL_SEXPR: {
            for (int i = 0; i < self->count; i++) {
                val_del(self->cell[i]);
            }
            free(self->cell);
            break;
        }
    }
    free(self);
}
