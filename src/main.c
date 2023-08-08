#include "builtins.h"
#include "common.h"
#include "eval.h"
#include "printing.h"
#include "reading.h"
#include "values.h"

//=== DECLARATIONS =============================================================
//--- Functions ----------------------------------------------------------------
void cli_interpreter(lenv_t* e);
void file_interpreter(lenv_t* e, int argc, char** argv);

//--- Variables ----------------------------------------------------------------
mpc_parser_t* number;
mpc_parser_t* symbol;
mpc_parser_t* sexpr;
mpc_parser_t* qexpr;
mpc_parser_t* string;
mpc_parser_t* comment;
mpc_parser_t* expr;
mpc_parser_t* lispy;

//=== MAIN METHOD ==============================================================

int main(int argc, char** argv) {
    number = mpc_new("number");
    symbol = mpc_new("symbol");
    sexpr = mpc_new("sexpr");
    qexpr = mpc_new("qexpr");
    string = mpc_new("string");
    comment = mpc_new("comment");
    expr = mpc_new("expr");
    lispy = mpc_new("lispy");
    mpca_lang(
        MPCA_LANG_DEFAULT,
        "                                                    \
            number  : /-?[0-9]+/ ;                           \
            symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&\\%^]+/ ; \
            sexpr   : '(' <expr>* ')' ;                      \
            qexpr   : '{' <expr>* '}' ;                      \
            string  : /\"(\\\\.|[^\"])*\"/ ;                 \
            comment : /;[^\\r\\n]*/ ;                        \
            expr    : <number>  | <symbol> | <string>        \
                    | <comment> | <sexpr>  | <qexpr>;        \
            lispy   : /^/ <expr>* /$/ ;                      \
        ",
        number,
        symbol,
        sexpr,
        qexpr,
        string,
        comment,
        expr,
        lispy
    );
    lenv_t* e = lenv_new();
    lenv_add_builtins(e);
    if (argc >= 2) {
        file_interpreter(e, argc, argv);
    } else {
        cli_interpreter(e);
    }
    mpc_cleanup(8, number, symbol, sexpr, qexpr, string, comment, expr, lispy);
    return 0;
}

void cli_interpreter(lenv_t* e) {
    printf("Lispy 1.0\n");
    printf("Press Ctrcl+c to exit.\n");
    while (true) {
        char* input = readline(">>> ");
        add_history(input);
        if (strcmp(input, "exit") == 0) {
            free(input);
            break;
        }
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, lispy, &r)) {
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
    return;
}

void file_interpreter(lenv_t* e, int argc, char** argv) {
    // loop over each supplied filename, starting from 1
    for (int i = 1; i < argc; i++) {
        // Argument list with a single argument, the filename
        lval_t* args = lval_add(lval_sexpr(), lval_str(argv[i]));

        // Pass to builtin load and get the result
        lval_t* x = builtin_load(e, args);

        // If the result is an error, be sure to print it
        if (x->type == LVAL_ERR) {
            lval_println(x);
        }
        lval_del(x);
    }
}

mpc_parser_t* get_lispy_parser(void) {
    return lispy;
}
