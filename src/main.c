/// @mainpage Lispy Interpreter
///
/// Lispy is a functional language inspired by Lisp. The interpreter is written
/// in C and is based on "Build Your Own Lisp" by Daniel Holden and uses his mpc
/// library.
///
/// @author edd255
/// @version 0.1

///
/// @file main.c
///
/// @brief This file contains the main function which sets up the logger and the
/// parser and sets up the command line arguments.

#include <editline/readline.h>

#include "builtins.h"
#include "common.h"
#include "deps/argparse/argparse.h"
#include "deps/logger/log.h"
#include "deps/mpc/mpc.h"
#include "eval.h"
#include "io.h"

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
/// Parses integer and decimal number
const mpc_parser_t* number;

/// Parses a set of special characters and symbols
const mpc_parser_t* symbol;

/// Parser symbolic expressions
const mpc_parser_t* sexpr;

/// Parser quoted expressions
const mpc_parser_t* qexpr;

/// Parses strings
const mpc_parser_t* string;

/// Parses comments
const mpc_parser_t* comment;

/// Parses expressions (which can be numbers, symbols, strings, comments, s- and q-expressions)
const mpc_parser_t* expr;

/// Parses lists of expressions
mpc_parser_t* lispy;

//--- Command-line argument parsing --------------------------------------------
/// A pointer to the file to interpret
char* filename = NULL;

/// Indicates whether to use the stdlib or not
int no_stdlib = 0;

/// Indicates whether to print logs, or to write them in a file
int print_logs = 0;

/// The usage of the binary is printed out in case the binary gets a wrong
/// format of arguments
static const char* const usages[] = {
    "lispy --filename=<FILENAME> --nostdlib --print_logs ",
    NULL,
};

//=== MAIN METHOD ==============================================================

/// @brief The main function of the interpreter
///
/// The main method sets up the logger, the command-line parser and the parser
/// for the interpreter. The user has the option to run specific files, to
/// include the standard library and to just go to the cli interpreter.
///
/// @param argc The number of arguments given to the binary, including the
/// binary name.
/// @param argv The array containing the command-line arguments.
/// @return 0 if everything ran without problems
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

/// @brief Sets up the command-line argument parser.
///
/// This function sets up the command-line argument parser. In case, that wrong
/// arguments are given, the usage is repeated.
/// @param argc The number of arguments
/// @param argv The array containing the arguments
void parse_args(int argc, const char** argv) {
    // Command-line parsing
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Basic options"),
        OPT_STRING('f', "filename", &filename, "lispy file to run", NULL, 0, 0),
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
    (void)argparse_parse(&argparse, argc, argv);
}

/// @brief Cleans up the memory used by the interpreter.
///
/// This function cleans up the memory used by the parsers, as well as the
/// standard library if loaded, and the environment.
///
/// @param std A pointer to the standard-library (if loaded, else NULL)
/// @param env A pointer to the used environment
void cleanup(lval_t* std, lenv_t* env) {
    mpc_cleanup(8, number, symbol, sexpr, qexpr, string, comment, expr, lispy);
    if (NULL != std) {
        lval_del(std);
    }
    lenv_del(env);
}

/// @brief Loads the standard library and returns a pointer to it.
///
/// This function loads the standard library and returns a pointer to it. The
/// path to the stdlib is hardcoded.
///
/// @param env The environment into which the standard library should be loaded
/// @return A pointer to the standard library.
lval_t* get_stdlib(lenv_t* env) {
    lval_t* standard =
        lval_add(lval_sexpr(), lval_str("/usr/local/lib/lispy/stdlib.lspy"));
    lval_t* std = builtin_load(env, standard);
    return std;
}

/// @brief A REPL for Lispy.
///
/// This function realizes a REPL for Lispy which first prints out version and
/// exiting information, reads input from the user, evaluates it, prints out the
/// result and continues.
///
/// \todo Include a print_info() method that prints out Lispy version and
/// exiting as well as build information (date, time, libc version, compiler
/// version).
///
/// @param env The environment from which we get and where we store variables
void cli_interpreter(lenv_t* env) {
    printf("Lispy 0.1\n");
    printf("Press Ctrl+c or type 'exit' to exit.\n");
    while (true) {
        char* input = readline(">>> ");
        add_history(input);
        if (0 == strcmp(input, "exit")) {
            free(input);
            break;
        }
        mpc_result_t parse_result;
        if (mpc_parse("<stdin>", input, lispy, &parse_result)) {
            lval_t* read_result = lval_read(parse_result.output);
            assert(NULL != read_result);
            lval_t* eval_result = lval_eval(env, read_result);
            lval_println(eval_result);
            lval_del(eval_result);
            mpc_ast_delete(parse_result.output);
        } else {
            mpc_err_print(parse_result.error);
            mpc_err_delete(parse_result.error);
        }
        free(input);
    }
}

/// @brief Interprets a given file.
///
/// This function runs the file interpreter, the second option, next to the
/// command-line interpreter.
///
/// @param env The environment which we use for interpreting the file
/// @param file The file to interpret
void file_interpreter(lenv_t* env, const char* file) {
    // Argument list with a single argument, the filename
    lval_t* argv_str = lval_str(file);
    lval_t* args = lval_add(lval_sexpr(), argv_str);

    // Pass to builtin load and get the result
    lval_t* x = builtin_load(env, args);

    // If the result is an error, be sure to print it
    if (LVAL_ERR == x->type) {
        lval_println(x);
    }
    lval_del(x);
}

/// @brief Sets up the mpc parser
///
/// This function sets up the parser using the mpc library by Daniel Holden. The
/// variables which contain the rules are documented itself.
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

/// @brief Sets up an environment which contains all builtins.
/// @return An environment which contains all builtin methods
lenv_t* set_env(void) {
    lenv_t* env = lenv_new();
    lenv_add_builtins(env);
    return env;
}

/// @brief Sets up the log-file in the XDG cache directory.
///
/// This function sets up the log-file in the XDG cache directory if the
/// directory exists.
///
/// \todo Make the function portable, and choose an alternative directory in
/// case XDG_CACHE_HOME does not exist.
///
/// @return A pointer to the log file.
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

/// @brief Returns the lispy parser.
mpc_parser_t* get_lispy_parser(void) {
    return lispy;
}
