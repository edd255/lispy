#include <editline/readline.h>

#include "builtins.h"
#include "common.h"
#include "deps/argparse/argparse.h"
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
const mpc_parser_t* number;
const mpc_parser_t* symbol;
const mpc_parser_t* sexpr;
const mpc_parser_t* qexpr;
const mpc_parser_t* string;
const mpc_parser_t* comment;
const mpc_parser_t* expr;
mpc_parser_t* lispy;

//--- Command-line argument parsing --------------------------------------------
char* file = NULL;
int no_stdlib = 0;
int print_logs = 0;

static const char* const usages[] = {
    "lispy --filename=<FILENAME> --nostdlib --print_logs ",
    NULL,
};

//=== MAIN METHOD ==============================================================

int main(int argc, const char** argv) {
    // Parse arguments and set up logfile, if necessary
    FILE* log_file = NULL;
    parse_args(argc, argv);
    if (0 == print_logs) {
        log_file = prepare_logfile();
        log_add_fp(log_file, 0);
        log_set_quiet(true);
    }

    // Set up the interpreter
    setup_parser();
    lenv_t* env = set_env();
    lval_t* std = NULL;
    if (0 == no_stdlib) {
        std = get_stdlib(env);
    }
    if (NULL != file) {
        file_interpreter(env, file);
    } else {
        cli_interpreter(env);
    }
    cleanup(std, env);
    return 0;
}

void parse_args(int argc, const char** argv) {
    // Command-line parsing
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Basic options"),
        OPT_STRING('f', "filename", &file, "lispy file to run", NULL, 0, 0),
        OPT_BOOLEAN('n', "no_stdlib", &no_stdlib, "exclude stdlib", NULL, 0, 0),
        OPT_BOOLEAN(
            'w',
            "print_logs",
            &print_logs,
            "print logs to stderr",
            NULL,
            0,
            0
        ),
        OPT_END(),
    };
    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nLispy Interpreter", "");
    argc = argparse_parse(&argparse, argc, argv);
    if (0 != argc) {
        printf("argc: %d\n", argc);
        for (int i = 0; i < argc; i++) {
            printf("argv[%d]: %s\n", i, argv[i]);
        }
    }
}

void cleanup(lval_t* std, lenv_t* env) {
    mpc_cleanup(8, number, symbol, sexpr, qexpr, string, comment, expr, lispy);
    if (NULL != std) {
        lval_del(std);
    }
    lenv_del(env);
}

lval_t* get_stdlib(lenv_t* env) {
    lval_t* standard =
        lval_add(lval_sexpr(), lval_str("/usr/local/lib/lispy/stdlib.lspy"));
    lval_t* std = builtin_load(env, standard);
    return std;
}

void cli_interpreter(lenv_t* e) {
    printf("Lispy 1.0\n");
    printf("Press Ctrl+c to exit.\n");
    while (true) {
        char* input = readline(">>> ");
        add_history(input);
        if (0 == strcmp(input, "exit")) {
            free(input);
            break;
        }
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, lispy, &r)) {
            lval_t* y = lval_read(r.output);
            assert(NULL != y);
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
}

void file_interpreter(lenv_t* e, const char* file) {
    // Argument list with a single argument, the filename
    lval_t* argv_str = lval_str(file);
    lval_t* args = lval_add(lval_sexpr(), argv_str);

    // Pass to builtin load and get the result
    lval_t* x = builtin_load(e, args);

    // If the result is an error, be sure to print it
    if (LVAL_ERR == x->type) {
        lval_println(x);
    }
    lval_del(x);
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
        "                                                              \
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
    FILE* log = NULL;
    char* cache_dir = getenv("XDG_CACHE_HOME");
    if (NULL == cache_dir) {
        free(cache_dir);
        log_debug("XDG_CACHE_HOME not set");
        return NULL;
    }
    char* log_file = malloc(strlen(cache_dir) + strlen("/lispy/lispy.log") + 1);
    strcpy(log_file, cache_dir);
    strcat(log_file, "/lispy/lispy.log");
    log = fopen(log_file, "we");
    free(log_file);
    return log;
}

mpc_parser_t* get_lispy_parser(void) {
    return lispy;
}
