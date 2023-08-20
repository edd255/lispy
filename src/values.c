#include "values.h"

#include "builtins.h"
#include "common.h"

//=== VALUES ===================================================================

lval_t* lval_copy(const lval_t* val) {
    assert(NULL != val);

    lval_t* x = MALLOC(sizeof(lval_t));
    x->type = val->type;
    switch (val->type) {
        // Copy Functions and Numbers Directly
        case LVAL_FN: {
            if (val->builtin) {
                x->builtin = val->builtin;
            } else {
                x->builtin = NULL;
                x->env = lenv_copy(val->env);
                x->formals = lval_copy(val->formals);
                x->body = lval_copy(val->body);
            }
            break;
        }
        case LVAL_NUM: {
            x->num = val->num;
            break;
        }
        case LVAL_DEC: {
            x->dec = val->dec;
            break;
        }
        // Copy Strings using malloc and strcpy
        case LVAL_ERR: {
            x->err = MALLOC(strlen(val->err) + 1);
            strcpy(x->err, val->err);
            break;
        }
        case LVAL_SYM: {
            x->sym = MALLOC(strlen(val->sym) + 1);
            strcpy(x->sym, val->sym);
            break;
        }
        case LVAL_STR: {
            x->str = MALLOC(strlen(val->str) + 1);
            strcpy(x->str, val->str);
            break;
        }
        // Copy Lists by copying each sub-expression
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            x->count = val->count;
            x->cell = MALLOC(sizeof(lval_t*) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = lval_copy(val->cell[i]);
            }
            break;
    }
    return x;
}

lval_t* lval_add(lval_t* val, lval_t* x) {
    assert(NULL != val);
    assert(NULL != x);

    val->count++;
    val->cell = REALLOC(val->cell, sizeof(lval_t*) * val->count);
    val->cell[val->count - 1] = x;
    return val;
}

lval_t* lval_join(lval_t* x, lval_t* y) {
    assert(NULL != x);
    assert(NULL != y);

    // For each cell in 'y' add it to 'x'
    for (int i = 0; i < y->count; i++) {
        assert(NULL != y->cell[i]);
        x = lval_add(x, y->cell[i]);
    }
    // Delete the empty 'y' and return 'x'
    free(y->cell);
    free(y);
    return x;
}

lval_t* lval_pop(lval_t* val, const int idx) {
    assert(NULL != val);

    // Find the item at "i"
    lval_t* x = val->cell[idx];

    // Shift memory after the item at "i" over the top
    memmove(
        &val->cell[idx],
        &val->cell[idx + 1],
        sizeof(lval_t*) * (val->count - idx - 1)
    );

    // Decrease the count of items in the list
    val->count--;

    // Reallocate the memory used
    val->cell = REALLOC(val->cell, sizeof(lval_t*) * val->count);
    return x;
}

lval_t* lval_take(lval_t* val, const int idx) {
    assert(NULL != val);

    lval_t* x = lval_pop(val, idx);
    lval_del(val);
    return x;
}

lval_t* lval_call(lenv_t* env, lval_t* fn, lval_t* a) {
    assert(NULL != env);
    assert(NULL != fn);
    assert(NULL != a);

    // If Builtin, then simply call that
    if (fn->builtin) {
        return fn->builtin(env, a);
    }
    // Record argument counts
    int given = a->count;
    int total = fn->formals->count;

    // While arguments still remain to be processed
    while (a->count) {
        // If we've run out of formal arguments to bind
        if (0 == fn->formals->count) {
            lval_del(a);
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
                lval_del(a);
                return lval_err(
                    "Function format invalid. Symbol '&' not followed by single symbol."
                );
            }

            // Next formal should be bound to remaining arguments
            lval_t* nsym = lval_pop(fn->formals, 0);
            lenv_put(fn->env, nsym, builtin_list(env, a));
            lval_del(sym);
            lval_del(nsym);
            break;
        }

        // Pop the next argument from the list
        lval_t* val = lval_pop(a, 0);

        // Bind a copy into the fn's environment
        lenv_put(fn->env, sym, val);

        // Delete symbol and value
        lval_del(sym);
        lval_del(val);
    }
    // Argument list is now bound so can be cleaned up
    lval_del(a);

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

int lval_eq(const lval_t* x, const lval_t* y) {
    assert(NULL != x);
    assert(NULL != y);

    // Different types are always unequal
    if (x->type != y->type) {
        return false;
    }
    // Compare based upon type
    switch (x->type) {
        // Compare number value
        case LVAL_NUM: {
            return (x->num == y->num);
        }
        case LVAL_DEC: {
            return (x->dec == y->dec);
        }
        // Compare string values
        case LVAL_ERR: {
            return 0 == strcmp(x->err, y->err);
        }
        case LVAL_SYM: {
            return 0 == strcmp(x->sym, y->sym);
        }
        // If builtin compare, otherwise compare formals and body
        case LVAL_FN: {
            if (x->builtin || y->builtin) {
                return x->builtin == y->builtin;
            }
            return lval_eq(x->formals, y->formals) && lval_eq(x->body, y->body);
        }
        case LVAL_STR: {
            return 0 == strcmp(x->str, y->str);
        }
        // If list compare every individual element
        case LVAL_QEXPR:
        case LVAL_SEXPR: {
            if (x->count != y->count) {
                return 0;
            }
            for (int i = 0; i < x->count; i++) {
                // If any element not equal then whole list not equal
                if (!lval_eq(x->cell[i], y->cell[i])) {
                    return false;
                }
            }
            // Otherwise lists must be equal;
            return true;
        }
    }
    return false;
}
