#include "main.h"

//=== CONSTRUCTORS =============================================================

/* Create a new number type lval_t */
lval_t* lval_num(long x) {
    lval_t* v = malloc(sizeof(lval_t));
    v -> type = LVAL_NUM;
    v -> num = x;
    return v;
}

lval_t* lval_err(char* fmt, ...) {
    lval_t* v = malloc(sizeof(lval_t));
    v -> type = LVAL_ERR;

    // Create a va list and initialize it
    va_list va;
    va_start(va, fmt);

    // Allocate 512 bytes of space
    v -> err = malloc(512);

    // printf the error string with a maximum of 511 character
    vsnprintf(v -> err, 511, fmt, va);

    // Reallocate to number of bytes actually used
    v -> err = realloc(v -> err, strlen(v -> err) + 1);

    // Cleanup our va list
    va_end(va);

    return v;
}

lval_t* lval_sym(char* s) {
    lval_t* v = malloc(sizeof(lval_t));
    v -> type = LVAL_SYM;
    v -> sym = malloc(strlen(s) + 1);
    strcpy(v -> sym, s);
    return v;
}

lval_t* lval_sexpr(void) {
    lval_t* v = malloc(sizeof(lval_t));
    v -> type = LVAL_SEXPR;
    v -> count = 0;
    v -> cell = NULL;
    return v;
}

lval_t* lval_qexpr(void) {
    lval_t* v = malloc(sizeof(lval_t));
    v -> type = LVAL_QEXPR;
    v -> count = 0;
    v -> cell = NULL;
    return v;
}

lval_t* lval_fn(lbuiltin_t fn) {
    lval_t* v = malloc(sizeof(lval_t));
    v -> type = LVAL_FN;
    v -> builtin = fn;
    return v;
}

lval_t* lval_lambda(lval_t* formals, lval_t* body) {
    lval_t* v = malloc(sizeof(lval_t));
    v -> type = LVAL_FN;

    // Set builtin to NULL
    v -> builtin = NULL;

    // Build new environment
    v -> env = lenv_new();

    // Set formals and body
    v -> formals = formals;
    v -> body = body;
    return v;
}

void lval_del(lval_t* v) {
    switch (v -> type) {
        // Do nothing special for number type
        case LVAL_NUM: {
            break;
        }
        // For Errors or Symbols free the string data
        case LVAL_ERR: {
            if (v -> err != NULL) {
                free(v -> err);
            }
            break;
        }
        case LVAL_SYM: {
            if (v -> sym != NULL) {
                free(v -> sym);
            }
            break;
        }
        case LVAL_FN: {
            if (!(v -> builtin)) {
                lenv_del(v -> env);
                lval_del(v -> formals);
                lval_del(v -> body);
            }
        }
        // If S-Expression or Q-Expression, then delete all elements inside
        case LVAL_QEXPR:
        case LVAL_SEXPR: {
            if (!(v -> cell)) {
                for (int i = 0; i < v -> count; i++) {
                    lval_del(v -> cell[i]);
                }
                free(v -> cell);
            }
            break;
        }
    }
    free(v);
}


//=== VALUES ===================================================================

lval_t* lval_copy(lval_t* v) {
    lval_t* x = malloc(sizeof(lval_t));
    x -> type = v -> type;
    switch (v -> type) {
        // Copy Functions and Numbers Directly
        case LVAL_FN: {
            if (v -> builtin) {
                x -> builtin = v -> builtin;
            } else {
                x -> builtin = NULL;
                x -> env = lenv_copy(v -> env);
                x -> formals = lval_copy(v -> formals);
                x -> body = lval_copy(v -> body);
            }
            break;
        }
        case LVAL_NUM: {
            x -> num = v -> num;
            break;
        }
        // Copy Strings using malloc and strcpy
        case LVAL_ERR: {
            x -> err = malloc(strlen(v -> err) + 1);
            strcpy(x -> err, v -> err);
            break;
        }
        case LVAL_SYM: {
            x -> sym = malloc(strlen(v -> sym) + 1);
            strcpy(x -> sym, v -> sym);
            break;
        }
        // Copy Lists by copying each sub-expression
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            x -> count = v -> count;
            x -> cell = malloc(sizeof(lval_t*) * x -> count);
            for (int i = 0; i < x -> count; i++) {
                x -> cell[i] = lval_copy(v -> cell[i]);
            }
            break;
    }
    return x;
}

lval_t* lval_add(lval_t* v, lval_t* x) {
    v -> count++;
    v -> cell = realloc(v -> cell, sizeof(lval_t*) * v -> count);
    v -> cell[v -> count-1] = x;
    return v;
}

lval_t* lval_join(lval_t* x, lval_t* y) {
    // For each cell in 'y' add it to 'x'
    while (y -> count) {
        x = lval_add(x, lval_pop(y, 0));
    }
    // Delete the empty 'y' and return 'x'
    lval_del(y);
    return x;
}

lval_t* lval_pop(lval_t* v, int i) {
    // Find the item at "i"
    lval_t* x = v -> cell[i];

    // Shift memory after the item at "i" over the top
    memmove(&v -> cell[i], &v -> cell[i+1], sizeof(lval_t*) * (v -> count-i-1));

    // Decrease the count of items in the list
    v -> count--;

    // Reallocate the memory used
    v -> cell = realloc(v -> cell, sizeof(lval_t*) * v -> count);
    return x;
}

lval_t* lval_take(lval_t* v, int i) {
    lval_t* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

lval_t* lval_call(lenv_t* e, lval_t* f, lval_t* a) {
    // If Builtin, then simply call that
    if (f -> builtin) {
        return f -> builtin(e, a);
    }
    // Record argument counts
    int given = a -> count;
    int total = f -> formals -> count;

    // While arguments still remain to be processed
    while (a -> count) {
        // If we've ran out of formal arguments to bind
        if (f -> formals -> count == 0) {
            lval_del(a);
            return lval_err(
                "Function passed too many arguments. Got %i. Expected %i.",
                given,
                total
            );
        }
        // Pop the first symbol from the formals
        lval_t* sym = lval_pop(f -> formals, 0);

        // Special case to deal with '&'
        if (strcmp(sym -> sym, "&") == 0) {
            // Ensure "&" is followed by another symbol
            if (f -> formals -> count != 1) {
                lval_del(a);
                return lval_err(
                    "Function format invalid. Symbol '&' not followed by single symbol."
                );
            }

            // Next formal should be bound to remaining arguments
            lval_t* nsym = lval_pop(f -> formals, 0);
            lenv_put(f -> env, nsym, builtin_list(e, a));
            lval_del(sym);
            lval_del(nsym);
            break;
        }

        // Pop the next argument from the list
        lval_t* val = lval_pop(a, 0);

        // Bind a copy into the fn's environment
        lenv_put(f -> env, sym, val);

        // Delete symbol and value
        lval_del(sym);
        lval_del(val);
    }
    // Argument list is now bound so can be cleaned up
    lval_del(a);

    if (
        f -> formals -> count > 0
        && strcmp(f -> formals -> cell[0] -> sym, "&") == 0
    ) {
        // Check to ensure that & is not passed invalidly
        if (f -> formals -> count != 2) {
            return lval_err(
                "Function format invalid. Symbol '&' not followed by single symbol."
            );
        }
        // Pop and delete '&' symbol
        lval_del(lval_pop(f -> formals, 0));

        // Pop next symbol and create empty list
        lval_t* sym = lval_pop(f -> formals, 0);
        lval_t* val = lval_qexpr();

        // Bind to environment and delete
        lenv_put(f -> env, sym, val);
        lval_del(sym);
        lval_del(val);
    }

    // If all formals have been bound evaluate
    if (f -> formals -> count == 0) {
        // Set environment parent to evaluation environment
        f -> env -> parent = e;

        // Evaluate and return
        return builtin_eval(
            f -> env,
            lval_add(lval_sexpr(), lval_copy(f -> body))
        );
    } else {
        // Otherwise return partially evaluated function
        return lval_copy(f);
    }
}


//=== PRINTING =================================================================

/* Print an lval_t */
void lval_print(lval_t* v) {
    switch (v -> type) {
        case LVAL_NUM: {
            printf("%li", v -> num);
            break;
        }
        case LVAL_ERR: {
            printf("Error: %s", v -> err);
            break;
        }
        case LVAL_SYM: {
            printf("%s", v -> sym);
            break;
        }
        case LVAL_FN: {
            if (v -> builtin) {
                printf("<builtin>");
            } else {
                printf("(\\ ");
                lval_print(v -> formals);
                putchar(' ');
                lval_print(v -> body);
                putchar(')');
            }
            break;
        }
        case LVAL_SEXPR: {
            lval_print_expr(v, '(', ')');
            break;
        }
        case LVAL_QEXPR: {
            lval_print_expr(v, '{', '}');
            break;
        }
    }
}

void lval_print_expr(lval_t* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v -> count; i++) {
        // Print Value contained within
        lval_print(v -> cell[i]);

        // Don't print trailing space if last element
        if (i != (v -> count-1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

/* Print an lval_t followed by a newline */
void lval_println(lval_t* v) {
    lval_print(v);
    putchar('\n');
}

char* ltype_name(int t) {
    switch (t) {
        case LVAL_FN: return "Function";
        case LVAL_NUM: return "Number";
        case LVAL_ERR: return "Error";
        case LVAL_SYM: return "Symbol";
        case LVAL_SEXPR: return "S-Expression";
        case LVAL_QEXPR: return "Q-Expression";
        default: return "Unknown";
    }
}


//=== ENVIRONMENT ==============================================================

lenv_t* lenv_new(void) {
    lenv_t* e = malloc(sizeof(lenv_t));
    e -> parent = NULL;
    e -> count = 0;
    e -> syms = NULL;
    e -> vals = NULL;
    return e;
}

void lenv_del(lenv_t* e) {
    for (int i = 0; i < e -> count; i++) {
        free(e -> syms[i]);
        lval_del(e -> vals[i]);
    }
    free(e -> syms);
    free(e -> vals);
    free(e);
}

lval_t* lenv_get(lenv_t* e, lval_t* k) {
    // Iterate over all items in environment
    for (int i = 0; i < e -> count; i++) {
        // Check if the stored string matches the symbol string. If it does,
        // return a copy of the value
        if (strcmp(e -> syms[i], k -> sym) == 0) {
            return lval_copy(e -> vals[i]);
        }
    }
    // If no symbol check in parent otherwise error
    if (e -> parent) {
        return lenv_get(e -> parent, k);
    } else {
        return lval_err("Unbound symbol '%s", k -> sym);
    }
}

void lenv_put(lenv_t* e, lval_t* k, lval_t* v) {
    // Iterate over all items in enviroment. This is to see if variable already
    // exists.
    for (int i = 0; i < e -> count; i++) {
        // If variable is found delete item at that position. And replace with
        // variable supplied by user.
        if (strcmp(e -> syms[i], k -> sym) == 0) {
            lval_del(e -> vals[i]);
            e -> vals[i] = lval_copy(v);
            return;
        }
    }
    // If no existing entry found allocate space for new entry
    e -> count++;
    e -> vals = realloc(e -> vals, sizeof(lval_t*) * e -> count);
    e -> syms = realloc(e -> syms, sizeof(char*) * e -> count);

    // Copy contents of lval and symbol string into new location
    e -> vals[e -> count - 1] = lval_copy(v);
    e -> syms[e -> count - 1] = malloc(strlen(k -> sym) + 1);
    strcpy(e -> syms[e -> count - 1], k -> sym);
}

lenv_t* lenv_copy(lenv_t* e) {
    lenv_t* n = malloc(sizeof(lenv_t));
    n -> parent = e -> parent;
    n -> count = e -> count;
    n -> syms = malloc(sizeof(char*) * (n -> count));
    n -> vals = malloc(sizeof(lval_t*) * (n -> count));
    for (int i = 0; i < e -> count; i++) {
        n -> syms[i] = malloc(strlen(e -> syms[i]) + 1);
        strcpy(n -> syms[i], e -> syms[i]);
        n -> vals[i] = lval_copy(e -> vals[i]);
    }
    return n;
}

void lenv_def(lenv_t* e, lval_t* k, lval_t* v) {
    // Iterate until e has no parent
    while (e -> parent) {
        e = e -> parent;
    }
    // Put value in e
    lenv_put(e, k, v);
}


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
}

//==== EVALUATION METHODS ======================================================

lval_t* lval_eval(lenv_t* e, lval_t* v) {
    if (v -> type == LVAL_SYM) {
        lval_t* x = lenv_get(e, v);
        lval_del(v);
        return x;
    }
    // Evaluate Sexpressions
    if (v -> type == LVAL_SEXPR) {
        return lval_eval_sexpr(e, v);
    }
    // All other lval_t types remain the same
    return v;
}

lval_t* lval_eval_sexpr(lenv_t* e, lval_t* v) {
    // Evaluate Children
    for (int i = 0; i < v -> count; i++) {
        v -> cell[i] = lval_eval(e, v -> cell[i]);
    }
    // Error Checking
    for (int i = 0; i < v -> count; i++) {
        if (v -> cell[i] -> type == LVAL_ERR) {
            return lval_take(v, i);
        }
    }
    // Empty Expression
    if (v -> count == 0) {
        return v;
    }
    // Single Expression
    if (v -> count == 1) {
        return lval_take(v, 0);
    }
    // Ensure first element is a function after evaluation
    lval_t* f = lval_pop(v, 0);
    if (f -> type != LVAL_FN) {
        lval_t* err = lval_err(
            "S-Expression starts with incorrect type. Got %s, Expected %s.",
            ltype_name(f -> type),
            ltype_name(LVAL_FN)
        );
        lval_del(f);
        lval_del(v);
        return err;
    }
    // Call builtin with operator
    lval_t* result = lval_call(e, f, v);
    lval_del(f);
    return result;
}


//==== READING =================================================================

lval_t* lval_read_num(mpc_ast_t* tree) {
    errno = 0;
    long x = strtol(tree -> contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval_t* lval_read(mpc_ast_t* tree) {
    // If Symbol or Number return conversion to that type
    if (strstr(tree -> tag, "number")) { return lval_read_num(tree); }
    if (strstr(tree -> tag, "symbol")) { return lval_sym(tree -> contents); }
    // If root (>) or sexpr then create empty list
    lval_t* x = NULL;
    if (strcmp(tree -> tag, ">") == 0) { x = lval_sexpr(); }
    if (strstr(tree -> tag, "sexpr"))  { x = lval_sexpr(); }
    if (strstr(tree -> tag, "qexpr"))  { x = lval_qexpr(); }
    // Fill this list with any valid expression contained within
    for (int i = 0; i < tree -> children_num; i++) {
        if (strcmp(tree -> children[i] -> contents, "(") == 0) { continue; }
        if (strcmp(tree -> children[i] -> contents, ")") == 0) { continue; }
        if (strcmp(tree -> children[i] -> contents, "{") == 0) { continue; }
        if (strcmp(tree -> children[i] -> contents, "}") == 0) { continue; }
        if (strcmp(tree -> children[i] -> tag,  "regex") == 0) { continue; }
        x = lval_add(x, lval_read(tree -> children[i]));
    }
    return x;
}


//=== MAIN METHOD ==============================================================

int main(void) {
    mpc_parser_t* number = mpc_new("number");
    mpc_parser_t* symbol = mpc_new("symbol");
    mpc_parser_t* expr   = mpc_new("expr");
    mpc_parser_t* sexpr  = mpc_new("sexpr");
    mpc_parser_t* qexpr  = mpc_new("qexpr");
    mpc_parser_t* clisp  = mpc_new("clisp");
    mpca_lang(
        MPCA_LANG_DEFAULT,
        "                                                       \
            number : /-?[0-9]+/ ;                               \
            symbol : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;         \
            sexpr  : '(' <expr>* ')' ;                          \
            qexpr  : '{' <expr>* '}' ;                          \
            expr   : <number> | <symbol> | <sexpr> | <qexpr> ;  \
            clisp: /^/ <expr>* /$/ ;                            \
        ",
        number,
        symbol,
        sexpr,
        qexpr,
        expr,
        clisp
    );
    printf("CLisp 0.1\n");
    printf("Press Ctrcl+c to exit.\n");
    lenv_t* e = lenv_new();
    lenv_add_builtins(e);
    while (true) {
        char* input = readline("clisp> ");
        add_history(input);
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, clisp, &r)) {
            lval_t* result = lval_eval(e, lval_read(r.output));
            lval_println(result);
            lval_del(result);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        free(input);
    }
    mpc_cleanup(6, number, symbol, expr, sexpr, qexpr, clisp);
    return 0;
}
