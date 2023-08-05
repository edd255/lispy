#include "common.h"
#include "builtins.h"
#include "eval.h"
#include "main.h"
#include "printing.h"
#include "reading.h"
#include "values.h"

//==== BUILTIN METHODS =========================================================

lval_t* builtin_list(lenv_t* e, lval_t* a) {
    UNUSED(e);
    a -> type = LVAL_QEXPR;
    return a;
}

lval_t* builtin_head(lenv_t* e, lval_t* a) {
    UNUSED(e);
    LASSERT_NUM("head", a, 1);
    LASSERT_TYPE("head", a, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("head", a, 0);

    // Take first argument
    lval_t* v = lval_take(a, 0);

    // Delete all elements that are not head and return
    while (v -> count > 1) {
        lval_del(lval_pop(v, 1));
    }
    return v;
}

lval_t* builtin_tail(lenv_t* e, lval_t* a) {
    UNUSED(e);
    LASSERT_NUM("tail", a, 1);
    LASSERT_TYPE("tail", a, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("tail", a, 0);

    // Take first argument
    lval_t* v = lval_take(a, 0);

    // Delete first element and return
    lval_del(lval_pop(v, 0));
    return v;
}

lval_t* builtin_eval(lenv_t* e, lval_t* a) {
    LASSERT_NUM("eval", a, 1);
    LASSERT_TYPE("eval", a, 0, LVAL_QEXPR);
    lval_t* x = lval_take(a, 0);
    x -> type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval_t* builtin_join(lenv_t* e, lval_t* a) {
    UNUSED(e);
    for (int i = 0; i < a -> count; i++) {
        LASSERT_TYPE("join", a, i, LVAL_QEXPR);
    }
    lval_t* x = lval_pop(a, 0);
    while (a -> count) {
        x = lval_join(x, lval_pop(a, 0));
    }
    lval_del(a);
    return x;
}

lval_t* builtin_op(lenv_t* e, lval_t* a, char* op) {
    UNUSED(e);

    // Ensure all arguments are numbers
    for (int i = 0; i < a -> count; i++) {
        LASSERT_TYPE(op, a, i, LVAL_NUM);
    }

    // Pop the first element
    lval_t* x = lval_pop(a, 0);

    // If no arguments and sub then perform unary negation
    if ((strcmp(op, "-") == 0) && a -> count == 0) {
        x -> num = -x -> num;
    }

    // While there are still elements remaining
    while (a -> count > 0) {
        // Pop the next element
        lval_t* y = lval_pop(a, 0);
        if (strcmp(op, "+") == 0) { x -> num += y -> num; }
        if (strcmp(op, "-") == 0) { x -> num -= y -> num; }
        if (strcmp(op, "*") == 0) { x -> num *= y -> num; }
        if (strcmp(op, "/") == 0) {
            if (y -> num == 0) {
                lval_del(x);
                lval_del(y);
                x = lval_err("Division By Zero!");
                break;
            }
            x -> num /= y -> num;
        }
        lval_del(y);
    }
    lval_del(a);
    return x;
}

lval_t* builtin_add(lenv_t* e, lval_t* a) {
    return builtin_op(e, a, "+");
}

lval_t* builtin_sub(lenv_t* e, lval_t* a) {
    return builtin_op(e, a, "-");
}

lval_t* builtin_mul(lenv_t* e, lval_t* a) {
    return builtin_op(e, a, "*");
}

lval_t* builtin_div(lenv_t* e, lval_t* a) {
    return builtin_op(e, a, "/");
}

lval_t* builtin_var(lenv_t* e, lval_t* a, char* fn) {
    LASSERT_TYPE(fn, a, 0, LVAL_QEXPR);

    // First argument is symbol list
    lval_t* syms = a -> cell[0];

    // Ensure all elements of first list are symbols
    for (int i = 0; i < syms -> count; i++) {
        LASSERT(
            a,
            (syms -> cell[i] -> type == LVAL_SYM),
            "Function '%s' cannot define non-symbol! Got %s. Expected %s.",
            fn,
            ltype_name(syms -> cell[i] -> type),
            ltype_name(LVAL_SYM)
        );
    }
    // Check correct number of symbols and values
    LASSERT(
        a,
        (syms -> count == a -> count - 1),
        "Function '%s' passed too many arguments for symbols. Got %i. Expected %i.",
        fn,
        syms -> count,
        a -> count - 1
    );
    // Assign copies of values to symbols
    for (int i = 0; i < syms -> count; i++) {
        // If 'def' define in globally. If 'put' define in locally.
        if (strcmp(fn, "def") == 0) {
            lenv_def(e, syms -> cell[i], a -> cell[i + 1]);
        }
        if (strcmp(fn, "=") == 0) {
            lenv_put(e, syms -> cell[i], a -> cell[i + 1]);
        }
    }
    lval_del(a);
    return lval_sexpr();
}

lval_t* builtin_def(lenv_t* e, lval_t* a) {
    return builtin_var(e, a, "def");
}

lval_t* builtin_put(lenv_t* e, lval_t* a) {
    return builtin_var(e, a, "=");
}

lval_t* builtin_lambda(lenv_t* e, lval_t* a) {
    UNUSED(e);

    // Check two arguments, each of which are Q-Expressions
    LASSERT_NUM("\\", a, 2);
    LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
    LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

    // Check first Q-Expression contains only Symbols
    for (int i = 0; i < a -> cell[0] -> count; i++) {
        LASSERT(
            a,
            (a -> cell[0] -> cell[i] -> type == LVAL_SYM),
            "Cannot define non-symbol. Got %s. Expected %s.",
            ltype_name(a -> cell[0] -> cell[i] -> type),
            ltype_name(LVAL_SYM)
        );
    }
    // Pop first two arguments and pass them to lval_lambda
    lval_t* formals = lval_pop(a, 0);
    lval_t* body = lval_pop(a, 0);
    lval_del(a);
    return lval_lambda(formals, body);
}

lval_t* builtin_gt(lenv_t* e, lval_t* a) {
    return builtin_ord(e, a, ">");
}

lval_t* builtin_lt(lenv_t* e, lval_t* a) {
    return builtin_ord(e, a, "<");
}

lval_t* builtin_ge(lenv_t* e, lval_t* a) {
    return builtin_ord(e, a, ">=");
}

lval_t* builtin_le(lenv_t* e, lval_t* a) {
    return builtin_ord(e, a, "<=");
}

lval_t* builtin_ord(lenv_t* e, lval_t* a, char* op) {
    UNUSED(e);
    LASSERT_NUM(op, a, 2);
    LASSERT_TYPE(op, a, 0, LVAL_NUM);
    LASSERT_TYPE(op, a, 1, LVAL_NUM);
    int r;
    if (strcmp(op, ">") == 0) {
        r = (a -> cell[0] -> num > a -> cell[1] -> num);
    }
    if (strcmp(op, "<") == 0) {
        r = (a -> cell[0] -> num < a -> cell[1] -> num);
    }
    if (strcmp(op, ">=") == 0) {
        r = (a -> cell[0] -> num >= a -> cell[1] -> num);
    }
    if (strcmp(op, "<=") == 0) {
        r = (a -> cell[0] -> num <= a -> cell[1] -> num);
    }

    lval_del(a);
    return lval_num(r);
}

lval_t* builtin_cmp(lenv_t* e, lval_t* a, char* op) {
    UNUSED(e);
    LASSERT_NUM(op, a, 2);
    int r;
    if (strcmp(op, "==") == 0) {
        r = lval_eq(a -> cell[0], a -> cell[1]);
    }
    if (strcmp(op, "!=") == 0) {
        r = !lval_eq(a -> cell[0], a -> cell[1]);
    }
    lval_del(a);
    return lval_num(r);
}

lval_t* builtin_eq(lenv_t* e, lval_t* a) {
    return builtin_cmp(e, a, "==");
}

lval_t* builtin_ne(lenv_t* e, lval_t* a) {
    return builtin_cmp(e, a, "!=");
}

lval_t* builtin_if(lenv_t* e, lval_t* a) {
    LASSERT_NUM("if", a, 3);
    LASSERT_TYPE("if", a, 0, LVAL_NUM);
    LASSERT_TYPE("if", a, 1, LVAL_QEXPR);
    LASSERT_TYPE("if", a, 2, LVAL_QEXPR);

    // Mark both expressions as evaluable
    lval_t* x;
    a -> cell[1] -> type = LVAL_SEXPR;
    a -> cell[2] -> type = LVAL_SEXPR;

    if (a -> cell[0] -> num) {
        // If condition is true evaluate first expression
        x = lval_eval(e, lval_pop(a, 1));
    } else {
        // Otherwise evaluate second expression
        x = lval_eval(e, lval_pop(a, 2));
    }
    // Delete argument list and return
    lval_del(a);
    return x;
}

lval_t* builtin_load(lenv_t* e, lval_t* a) {
    LASSERT_NUM("load", a, 1);
    LASSERT_TYPE("load", a, 0, LVAL_STR);

    // Parse file given by string name
    mpc_result_t r;
    mpc_parser_t* lispy = get_lispy_parser();
    if (mpc_parse_contents(a -> cell[0] -> str, lispy, &r)) {
        // Read contents
        lval_t* expr = lval_read(r.output);
        mpc_ast_delete(r.output);

        // Evaluate each expression
        while (expr -> count) {
            lval_t* x = lval_eval(e, lval_pop(expr, 0));

            // If evaluation leads to error print it
            if (x -> type == LVAL_ERR) {
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
    UNUSED(e);

    // Print each argument followed by a space
    for (int i = 0; i < a -> count; i++) {
        lval_print(a -> cell[i]);
        putchar(' ');
    }
    // Print a newline and delete arguments
    putchar('\n');
    lval_del(a);
    return lval_sexpr();
}

lval_t* builtin_error(lenv_t* e, lval_t* a) {
    UNUSED(e);
    LASSERT_NUM("error", a, 1);
    LASSERT_TYPE("error", a, 0, LVAL_STR);

    // Construct error from first argument
    lval_t* err = lval_err(a -> cell[0] -> str);

    // Delete arguments and return
    lval_del(a);
    return err;
}

void lenv_add_builtin(lenv_t* e, char* name, lbuiltin_t fn) {
    lval_t* k = lval_sym(name);
    lval_t* v = lval_fn(fn);
    lenv_put(e, k, v);
    lval_del(k);
    lval_del(v);
}

void lenv_add_builtins(lenv_t* e) {
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

    // Comparison functions
    lenv_add_builtin(e, "if", builtin_if);
    lenv_add_builtin(e, "==", builtin_eq);
    lenv_add_builtin(e, "!=", builtin_ne);
    lenv_add_builtin(e, ">",  builtin_gt);
    lenv_add_builtin(e, "<",  builtin_lt);
    lenv_add_builtin(e, ">=", builtin_ge);
    lenv_add_builtin(e, "<=", builtin_le);

    // String Functions
    lenv_add_builtin(e, "load",  builtin_load);
    lenv_add_builtin(e, "error", builtin_error);
    lenv_add_builtin(e, "print", builtin_print);
}

