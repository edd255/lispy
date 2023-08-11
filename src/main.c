#include "../deps/argparse/argparse.h"
#include "builtins.h"
#include "common.h"
#include "eval.h"
#include "printing.h"
#include "reading.h"
#include "values.h"

//=== DECLARATIONS =============================================================
//--- Functions ----------------------------------------------------------------
void cli_interpreter(lenv_t* e);
void file_interpreter(lenv_t* e, const char* file);
void setup_parser(void);
lenv_t* set_env(void);
void cleanup(lval_t* std, lenv_t* e);
lval_t* get_stdlib(lenv_t* e);
void parse_args(int argc, const char** argv);
FILE* prepare_logfile(void);

//--- Variables ----------------------------------------------------------------
mpc_parser_t* number;
mpc_parser_t* symbol;
mpc_parser_t* sexpr;
mpc_parser_t* qexpr;
mpc_parser_t* string;
mpc_parser_t* comment;
mpc_parser_t* expr;
mpc_parser_t* lispy;

//--- Command-line argument parsing --------------------------------------------
char* file = NULL;
int no_stdlib = 0;
int write_logs = 0;

static const char* const usages[] = {
    "lispy --filename=<FILENAME> --nostdlib --write_logs ",
    NULL,
};

//=== MAIN METHOD ==============================================================

int main(int argc, const char** argv) {
    // Parse arguments and set up logfile, if necessary
    setup_parser();
    FILE* log_file = NULL;
    parse_args(argc, argv);
    if (write_logs != 0) {
        log_file = prepare_logfile();
        printf("write logs to file %p\n", (void*)log_file);
        log_add_fp(log_file, 0);
    }

    // Set up the interpreter
    lenv_t* e = set_env();
    lval_t* std = NULL;
    if (no_stdlib == 0) {
        std = get_stdlib(e);
    }
    if (file != NULL) {
        file_interpreter(e, file);
    } else {
        cli_interpreter(e);
    }
    cleanup(std, e);
    log_debug("Exiting...");
    return 0;
}

void parse_args(int argc, const char** argv) {
    // Command-line parsing
    log_debug("Parsing arguments...");
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Basic options"),
        OPT_STRING('f', "filename", &file, "lispy file to run", NULL, 0, 0),
        OPT_BOOLEAN('n', "no_stdlib", &no_stdlib, "exclude stdlib", NULL, 0, 0),
        OPT_BOOLEAN(
            'w',
            "write_logs",
            &write_logs,
            "write logs to file",
            NULL,
            0,
            0
        ),
        OPT_END(),
    };
    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nLispy Interpreter.", "");
    argc = argparse_parse(&argparse, argc, argv);
}

void cleanup(lval_t* std, lenv_t* e) {
    log_debug("Cleaning up...");
    mpc_cleanup(8, number, symbol, sexpr, qexpr, string, comment, expr, lispy);
    if (NULL != std) {
        lval_del(std);
    }
    lenv_del(e);
}

lval_t* get_stdlib(lenv_t* e) {
    log_debug("Loading stdlib");
    lval_t* standard =
        lval_add(lval_sexpr(), lval_str("/usr/local/lib/lispy/stdlib.lspy"));
    lval_t* std = builtin_load(e, standard);
    log_debug("Loaded stdlib");
    return std;
}

void cli_interpreter(lenv_t* e) {
    printf("Lispy 1.0\n");
    printf("Press Ctrl+c to exit.\n");
    while (true) {
        char* input = readline(">>> ");
        add_history(input);
        if (strcmp(input, "exit") == 0) {
            free(input);
            break;
        }
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, lispy, &r)) {
            lval_t* y = lval_read(r.output);
            assert(y != NULL);
            lval_t* result = lval_eval(e, y);
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

void file_interpreter(lenv_t* e, const char* file) {
    // Argument list with a single argument, the filename
    lval_t* argv_str = lval_str(file);
    lval_t* args = lval_add(lval_sexpr(), argv_str);

    // Pass to builtin load and get the result
    lval_t* x = builtin_load(e, args);

    // If the result is an error, be sure to print it
    if (x->type == LVAL_ERR) {
        lval_println(x);
    }
    lval_del(x);
    return;
}

void setup_parser(void) {
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
        "                                                      \
            number  : /[+-]?(([0-9]*[.])?[0-9]+|[0-9]+([.][0-9]*)?)/ ; \
            symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&\\%^]+/ ;           \
            sexpr   : '(' <expr>* ')' ;                                \
            qexpr   : '{' <expr>* '}' ;                                \
            string  : /\"(\\\\.|[^\"])*\"/ ;                           \
            comment : /;[^\\r\\n]*/ ;                                  \
            expr    : <number>  | <symbol> | <string>                  \
                    | <comment> | <sexpr>  | <qexpr>;                  \
            lispy   : /^/ <expr>* /$/ ;                                \
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
}

lenv_t* set_env(void) {
    lenv_t* e = lenv_new();
    lenv_add_builtins(e);
    return e;
}

FILE* prepare_logfile(void) {
    FILE* log_file;
    log_file = fopen("lispy.log", "w");
    return log_file;
}

mpc_parser_t* get_lispy_parser(void) {
    return lispy;
}
