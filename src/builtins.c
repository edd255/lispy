#include "builtins.h"

#include <math.h>

#include "common.h"
#include "eval.h"
#include "main.h"
#include "printing.h"
#include "reading.h"
#include "values.h"

//==== HELPER METHODS ==========================================================

enum LOP { LOP_ADD, LOP_SUB, LOP_MUL, LOP_DIV, LOP_MOD, LOP_POW, LOP_UNKNOWN };

typedef struct op_map_t op_map_t;
struct op_map_t {
    char* key;
    enum LOP value;
};

const op_map_t op_map[] = {
    {"+", LOP_ADD},
    {"-", LOP_SUB},
    {"*", LOP_MUL},
    {"/", LOP_DIV},
    {"%", LOP_MOD},
    {"^", LOP_POW},
};

#define NUMBER_OF_OPS (int)(sizeof(op_map) / sizeof(op_map_t))

enum LOP op_from_string(char* key) {
    for (int i = 0; i < NUMBER_OF_OPS; i++) {
        op_map_t op = op_map[i];
        if (strcmp(op.key, key) == 0) {
            return op.value;
        }
    }
    return LOP_UNKNOWN;
}

long power_long(long base, long exponent) {
    if (exponent == 0) {
        return 1;
    }
    if (exponent == 1) {
        return base;
    }
    long result = 1;
    bool neg = false;
    if (exponent < 0) {
        neg = true;
    }
    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result *= base;
        }
        base *= base;
        exponent /= 2;
    }
    return neg ? 1 / result : result;
}

//==== BUILTIN METHODS =========================================================

void lenv_add_builtins(lenv_t* e) {
    assert(e != NULL);

    // Variable functions
    lenv_add_builtin(e, "\\", builtin_lambda);
    lenv_add_builtin(e, "def", builtin_def);
    lenv_add_builtin(e, "=", builtin_put);

    // List functions
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "join", builtin_join);

    // Mathematical functions
    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);
    lenv_add_builtin(e, "%", builtin_mod);
    lenv_add_builtin(e, "^", builtin_pow);

    // Comparison functions
    lenv_add_builtin(e, "if", builtin_if);
    lenv_add_builtin(e, "==", builtin_eq);
    lenv_add_builtin(e, "!=", builtin_ne);
    lenv_add_builtin(e, ">", builtin_gt);
    lenv_add_builtin(e, "<", builtin_lt);
    lenv_add_builtin(e, ">=", builtin_ge);
    lenv_add_builtin(e, "<=", builtin_le);

    // String Functions
    lenv_add_builtin(e, "load", builtin_load);
    lenv_add_builtin(e, "error", builtin_error);
    lenv_add_builtin(e, "print", builtin_print);
}

void lenv_add_builtin(lenv_t* e, char* name, lbuiltin_t fn) {
    assert(e != NULL);
    assert(name != NULL);
    assert(fn != NULL);

    lval_t* k = lval_sym(name);
    lval_t* v = lval_fn(fn);
    lenv_put(e, k, v);
    lval_del(k);
    lval_del(v);
}

//==== Variable functions ======================================================
lval_t* builtin_var(lenv_t* e, lval_t* a, char* fn) {
    assert(e != NULL);
    assert(a != NULL);
    assert(fn != NULL);
    LASSERT_TYPE(fn, a, 0, LVAL_QEXPR)

    // First argument is symbol list
    lval_t* syms = a->cell[0];

    // Ensure all elements of first list are symbols
    for (int i = 0; i < syms->count; i++) {
        LASSERT(
            a,
            (syms->cell[i]->type == LVAL_SYM),
            "Error in procedure %s. Function '%s' cannot define non-symbol! Got %s. Expected %s.",
            __func__,
            fn,
            ltype_name(syms->cell[i]->type),
            ltype_name(LVAL_SYM)
        )
    }
    // Check correct number of symbols and values
    LASSERT(
        a,
        (syms->count == a->count - 1),
        "Error in procedure %s. Function '%s' passed too many arguments for symbols. Got %i. Expected %i.",
        __func__,
        fn,
        syms->count,
        a->count - 1
    )
    // Assign copies of values to symbols
    for (int i = 0; i < syms->count; i++) {
        // If 'def' define in globally. If 'put' define in locally.
        if (strcmp(fn, "def") == 0) {
            lenv_def(e, syms->cell[i], a->cell[i + 1]);
        }
        if (strcmp(fn, "=") == 0) {
            lenv_put(e, syms->cell[i], a->cell[i + 1]);
        }
    }
    lval_del(a);
    return lval_sexpr();
}

lval_t* builtin_lambda(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);
    UNUSED(e);

    // Check two arguments, each of which are Q-Expressions
    LASSERT_NUM("\\", a, 2)
    LASSERT_TYPE("\\", a, 0, LVAL_QEXPR)
    LASSERT_TYPE("\\", a, 1, LVAL_QEXPR)

    // Check first Q-Expression contains only Symbols
    for (int i = 0; i < a->cell[0]->count; i++) {
        LASSERT(
            a,
            (a->cell[0]->cell[i]->type == LVAL_SYM),
            "Error in procedure %s. Cannot define non-symbol. Got %s. Expected %s.",
            __func__,
            ltype_name(a->cell[0]->cell[i]->type),
            ltype_name(LVAL_SYM)
        )
    }
    // Pop first two arguments and pass them to lval_lambda
    lval_t* formals = lval_pop(a, 0);
    lval_t* body = lval_pop(a, 0);

    assert(formals != NULL);
    assert(body != NULL);

    lval_del(a);
    return lval_lambda(formals, body);
}

lval_t* builtin_def(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_var(e, a, "def");
}

lval_t* builtin_put(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_var(e, a, "=");
}

//==== List functions ==========================================================
lval_t* builtin_list(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);
    UNUSED(e);

    a->type = LVAL_QEXPR;
    return a;
}

lval_t* builtin_head(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);
    UNUSED(e);
    LASSERT_NUM(__func__, a, 1)
    LASSERT_TYPE(__func__, a, 0, LVAL_QEXPR)
    LASSERT_NOT_EMPTY(__func__, a, 0)

    // Take first argument
    lval_t* v = lval_take(a, 0);

    // Delete all elements that are not head and return
    while (v->count > 1) {
        lval_t* y = lval_pop(v, 1);
        assert(y != NULL);
        lval_del(y);
    }
    return v;
}

lval_t* builtin_tail(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);
    UNUSED(e);
    LASSERT_NUM(__func__, a, 1)
    LASSERT_TYPE(__func__, a, 0, LVAL_QEXPR)
    LASSERT_NOT_EMPTY(__func__, a, 0)

    // Take first argument
    lval_t* v = lval_take(a, 0);

    // Delete first element and return
    lval_t* y = lval_pop(v, 0);
    assert(y != NULL);
    lval_del(y);
    return v;
}

lval_t* builtin_eval(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);
    UNUSED(e);
    LASSERT_NUM(__func__, a, 1)
    LASSERT_TYPE(__func__, a, 0, LVAL_QEXPR)

    lval_t* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval_t* builtin_join(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);
    UNUSED(e);

    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE(__func__, a, i, LVAL_QEXPR)
    }
    lval_t* x = lval_pop(a, 0);
    assert(x != NULL);
    while (a->count) {
        lval_t* y = lval_pop(a, 0);
        assert(y != NULL);
        x = lval_join(x, y);
    }
    lval_del(a);
    return x;
}

//==== Mathematical functions ==================================================
lval_t* builtin_op(lenv_t* e, lval_t* a, char* op) {
    assert(e != NULL);
    assert(a != NULL);
    assert(op != NULL);
    UNUSED(e);
    LASSERT_TYPES(op, a, 0, LVAL_NUM, LVAL_DEC)

    // Ensure all arguments are numbers
    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE(op, a, i, a->cell[0]->type)
    }

    // Pop the first element
    lval_t* x = lval_pop(a, 0);
    assert(x != NULL);

    // If no arguments and sub then perform unary negation
    if ((strcmp(op, "-") == 0) && a->count == 0) {
        x->num = -x->num;
    }

    // While there are still elements remaining
    while (a->count > 0) {
        // Pop the next element
        lval_t* y = lval_pop(a, 0);
        assert(y != NULL);
        switch (op_from_string(op)) {
            case LOP_ADD: {
                switch (x->type) {
                    case LVAL_NUM: {
                        x->num += y->num;
                        break;
                    }
                    case LVAL_DEC: {
                        x->dec += y->dec;
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LOP_SUB: {
                switch (x->type) {
                    case LVAL_NUM: {
                        x->num -= y->num;
                        break;
                    }
                    case LVAL_DEC: {
                        x->dec -= y->dec;
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LOP_MUL: {
                switch (x->type) {
                    case LVAL_NUM: {
                        x->num *= y->num;
                        break;
                    }
                    case LVAL_DEC: {
                        x->dec *= y->dec;
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LOP_DIV: {
                if (y->num == 0) {
                    lval_del(x);
                    lval_del(y);
                    x = lval_err("Division By Zero!");
                    break;
                }
                switch (x->type) {
                    case LVAL_NUM: {
                        x->num /= y->num;
                        break;
                    }
                    case LVAL_DEC: {
                        x->dec /= y->dec;
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LOP_MOD: {
                if (y->num == 0) {
                    lval_del(x);
                    lval_del(y);
                    x = lval_err("Division By Zero!");
                    break;
                }
                switch (x->type) {
                    case LVAL_NUM: {
                        x->num %= y->num;
                        break;
                    }
                    case LVAL_DEC: {
                        lval_del(x);
                        lval_del(y);
                        x = lval_err("Modulo not allowed for decimals!");
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LOP_POW: {
                switch (x->type) {
                    case LVAL_NUM: {
                        x->num = power_long(x->num, y->num);
                        break;
                    }
                    case LVAL_DEC: {
                        x->dec = pow(x->dec, y->dec);
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LOP_UNKNOWN: {
                lval_del(y);
                break;
            }
        }
    }
    lval_del(a);
    return x;
}

lval_t* builtin_add(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_op(e, a, "+");
}

lval_t* builtin_sub(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_op(e, a, "-");
}

lval_t* builtin_mul(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_op(e, a, "*");
}

lval_t* builtin_div(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_op(e, a, "/");
}

lval_t* builtin_mod(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_op(e, a, "%");
}

lval_t* builtin_pow(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_op(e, a, "^");
}

//==== Comparison functions ====================================================
lval_t* builtin_if(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);
    UNUSED(e);
    LASSERT_NUM(__func__, a, 3)
    LASSERT_TYPE(__func__, a, 0, LVAL_NUM)
    LASSERT_TYPE(__func__, a, 1, LVAL_QEXPR)
    LASSERT_TYPE(__func__, a, 2, LVAL_QEXPR)

    // Mark both expressions as evaluable
    lval_t* x;
    a->cell[1]->type = LVAL_SEXPR;
    a->cell[2]->type = LVAL_SEXPR;

    if (a->cell[0]->num) {
        // If condition is true evaluate first expression
        lval_t* y = lval_pop(a, 1);
        assert(y != NULL);
        x = lval_eval(e, y);
    } else {
        // Otherwise evaluate second expression
        lval_t* y = lval_pop(a, 2);
        assert(y != NULL);
        x = lval_eval(e, y);
    }
    // Delete argument list and return
    lval_del(a);
    return x;
}

//---- Magnitude comparison functions ------------------------------------------
lval_t* builtin_ord(lenv_t* e, lval_t* a, char* op) {
    assert(e != NULL);
    assert(a != NULL);
    assert(op != NULL);
    UNUSED(e);
    LASSERT_NUM(op, a, 2)
    LASSERT_TYPE(op, a, 0, LVAL_NUM)
    LASSERT_TYPE(op, a, 1, LVAL_NUM)

    int r;
    if (strcmp(op, ">") == 0) {
        r = (a->cell[0]->num > a->cell[1]->num);
    } else if (strcmp(op, "<") == 0) {
        r = (a->cell[0]->num < a->cell[1]->num);
    } else if (strcmp(op, ">=") == 0) {
        r = (a->cell[0]->num >= a->cell[1]->num);
    } else if (strcmp(op, "<=") == 0) {
        r = (a->cell[0]->num <= a->cell[1]->num);
    } else {
        return lval_err(
            "Error during magnitude comparison: Neither >, <, >=, <= used!"
        );
    }

    lval_del(a);
    return lval_num(r);
}

lval_t* builtin_gt(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_ord(e, a, ">");
}

lval_t* builtin_lt(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_ord(e, a, "<");
}

lval_t* builtin_ge(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_ord(e, a, ">=");
}

lval_t* builtin_le(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_ord(e, a, "<=");
}

//---- Equality comparison functions -------------------------------------------
lval_t* builtin_cmp(lenv_t* e, lval_t* a, char* op) {
    assert(e != NULL);
    assert(a != NULL);
    assert(op != NULL);
    UNUSED(e);
    LASSERT_NUM(op, a, 2)

    int r;
    if (strcmp(op, "==") == 0) {
        r = lval_eq(a->cell[0], a->cell[1]);
    } else if (strcmp(op, "!=") == 0) {
        r = !lval_eq(a->cell[0], a->cell[1]);
    } else {
        return lval_err("Error during comparison: Neither == nor != used!");
    }
    lval_del(a);
    return lval_num(r);
}

lval_t* builtin_eq(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_cmp(e, a, "==");
}

lval_t* builtin_ne(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_cmp(e, a, "!=");
}

//==== String functions ========================================================
lval_t* builtin_load(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);
    LASSERT_NUM(__func__, a, 1)
    LASSERT_TYPE(__func__, a, 0, LVAL_STR)

    // Parse file given by string name
    mpc_result_t r;
    mpc_parser_t* lispy = get_lispy_parser();
    if (mpc_parse_contents(a->cell[0]->str, lispy, &r)) {
        // Read contents
        assert(r.output != NULL);
        lval_t* expr = lval_read(r.output);
        assert(expr != NULL);
        mpc_ast_delete(r.output);

        // Evaluate each expression
        while (expr->count) {
            lval_t* y = lval_pop(expr, 0);
            assert(y != NULL);
            lval_t* x = lval_eval(e, y);

            // If evaluation leads to error print it
            if (x->type == LVAL_ERR) {
                lval_println(x);
            }
            lval_del(x);
        }
        // Delete expressions and arguments
        lval_del(expr);
        lval_del(a);

        // Return empty list
        return lval_sexpr();
    } else {
        // Get parse error as string
        char* err_msg = mpc_err_string(r.error);
        mpc_err_delete(r.error);

        // Create new error message using it
        lval_t* err = lval_err("Could not load library %s", err_msg);
        free(err_msg);
        lval_del(a);

        // Cleanup and return error
        return err;
    }
}

lval_t* builtin_print(lenv_t* e, lval_t* a) {
    log_debug("%p %p", e, a);
    assert(e != NULL);
    assert(a != NULL);
    UNUSED(e);

    // Print each argument followed by a space
    for (int i = 0; i < a->count; i++) {
        lval_print(a->cell[i]);
        putchar(' ');
    }
    // Print a newline and delete arguments
    putchar('\n');
    log_debug("%s", ltype_name(a->type));
    for (int i = 0; i < a->count; i++) {
        log_debug("%p", a->cell[i]);
    }
    lval_del(a);
    return lval_sexpr();
}

lval_t* builtin_error(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);
    UNUSED(e);
    LASSERT_NUM(__func__, a, 1)
    LASSERT_TYPE(__func__, a, 0, LVAL_STR)

    // Construct error from first argument
    lval_t* err = lval_err(a->cell[0]->str);

    // Delete arguments and return
    lval_del(a);
    return err;
}
