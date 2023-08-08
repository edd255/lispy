#include "values.h"

#include "builtins.h"
#include "common.h"

//=== VALUES ===================================================================

lval_t* lval_copy(lval_t* v) {
    lval_t* x = malloc(sizeof(lval_t));
    x->type = v->type;
    switch (v->type) {
        // Copy Functions and Numbers Directly
        case LVAL_FN: {
            if (v->builtin) {
                x->builtin = v->builtin;
            } else {
                x->builtin = NULL;
                x->env = lenv_copy(v->env);
                x->formals = lval_copy(v->formals);
                x->body = lval_copy(v->body);
            }
            break;
        }
        case LVAL_NUM: {
            x->num = v->num;
            break;
        }
        case LVAL_DEC: {
            x->dec = v->dec;
            break;
        }
        // Copy Strings using malloc and strcpy
        case LVAL_ERR: {
            x->err = malloc(strlen(v->err) + 1);
            strcpy(x->err, v->err);
            break;
        }
        case LVAL_SYM: {
            x->sym = malloc(strlen(v->sym) + 1);
            strcpy(x->sym, v->sym);
            break;
        }
        case LVAL_STR: {
            x->str = malloc(strlen(v->str) + 1);
            strcpy(x->str, v->str);
            break;
        }
        // Copy Lists by copying each sub-expression
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            x->count = v->count;
            x->cell = malloc(sizeof(lval_t*) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = lval_copy(v->cell[i]);
            }
            break;
    }
    return x;
}

lval_t* lval_add(lval_t* v, lval_t* x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval_t*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

lval_t* lval_join(lval_t* x, lval_t* y) {
    // For each cell in 'y' add it to 'x'
    while (y->count) {
        x = lval_add(x, lval_pop(y, 0));
    }
    // Delete the empty 'y' and return 'x'
    lval_del(y);
    return x;
}

lval_t* lval_pop(lval_t* v, int i) {
    // Find the item at "i"
    lval_t* x = v->cell[i];

    // Shift memory after the item at "i" over the top
    memmove(&v->cell[i], &v->cell[i + 1], sizeof(lval_t*) * (v->count - i - 1));

    // Decrease the count of items in the list
    v->count--;

    // Reallocate the memory used
    v->cell = realloc(v->cell, sizeof(lval_t*) * v->count);
    return x;
}

lval_t* lval_take(lval_t* v, int i) {
    lval_t* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

lval_t* lval_call(lenv_t* e, lval_t* f, lval_t* a) {
    // If Builtin, then simply call that
    if (f->builtin) {
        return f->builtin(e, a);
    }
    // Record argument counts
    int given = a->count;
    int total = f->formals->count;

    // While arguments still remain to be processed
    while (a->count) {
        // If we've ran out of formal arguments to bind
        if (f->formals->count == 0) {
            lval_del(a);
            return lval_err(
                "Function passed too many arguments. Got %i. Expected %i.",
                given,
                total
            );
        }
        // Pop the first symbol from the formals
        lval_t* sym = lval_pop(f->formals, 0);

        // Special case to deal with '&'
        if (strcmp(sym->sym, "&") == 0) {
            // Ensure "&" is followed by another symbol
            if (f->formals->count != 1) {
                lval_del(a);
                return lval_err(
                    "Function format invalid. Symbol '&' not followed by single symbol."
                );
            }

            // Next formal should be bound to remaining arguments
            lval_t* nsym = lval_pop(f->formals, 0);
            lenv_put(f->env, nsym, builtin_list(e, a));
            lval_del(sym);
            lval_del(nsym);
            break;
        }

        // Pop the next argument from the list
        lval_t* val = lval_pop(a, 0);

        // Bind a copy into the fn's environment
        lenv_put(f->env, sym, val);

        // Delete symbol and value
        lval_del(sym);
        lval_del(val);
    }
    // Argument list is now bound so can be cleaned up
    lval_del(a);

    if (f->formals->count > 0 && strcmp(f->formals->cell[0]->sym, "&") == 0) {
        // Check to ensure that & is not passed invalidly
        if (f->formals->count != 2) {
            return lval_err(
                "Function format invalid. Symbol '&' not followed by single symbol."
            );
        }
        // Pop and delete '&' symbol
        lval_del(lval_pop(f->formals, 0));

        // Pop next symbol and create empty list
        lval_t* sym = lval_pop(f->formals, 0);
        lval_t* val = lval_qexpr();

        // Bind to environment and delete
        lenv_put(f->env, sym, val);
        lval_del(sym);
        lval_del(val);
    }

    // If all formals have been bound evaluate
    if (f->formals->count == 0) {
        // Set environment parent to evaluation environment
        f->env->parent = e;

        // Evaluate and return
        return builtin_eval(f->env, lval_add(lval_sexpr(), lval_copy(f->body)));
    } else {
        // Otherwise return partially evaluated function
        return lval_copy(f);
    }
}

int lval_eq(lval_t* x, lval_t* y) {
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
            return strcmp(x->err, y->err) == 0;
        }
        case LVAL_SYM: {
            return strcmp(x->sym, y->sym) == 0;
        }
        // If builtin compare, otherwise compare formals and body
        case LVAL_FN: {
            if (x->builtin || y->builtin) {
                return x->builtin == y->builtin;
            } else {
                return lval_eq(x->formals, y->formals)
                    && lval_eq(x->body, y->body);
            }
        }
        case LVAL_STR: {
            return strcmp(x->str, y->str) == 0;
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
                // Otherwise lists must be equal;
                return true;
            }
        }
    }
    return false;
}
