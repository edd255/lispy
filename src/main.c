/// @mainpage Lispy Interpreter
///
/// Lispy is a functional language inspired by Lisp. The interpreter is written
/// in C and is based on "Build Your Own Lisp" by Daniel Holden and uses his mpc
/// library.
///
/// @author edd255
/// @version 0.1

/// @file main.c
///
/// @brief This file contains the main function which sets up the logger and the
/// parser and sets up the command line arguments.

#include <editline/readline.h>

#include "builtins.h"
#include "common.h"
#include "deps/logger/log.h"
#include "deps/mpc/mpc.h"
#include "eval.h"
#include "io.h"
#include "utils/argparser.h"
#include "utils/logger.h"
#include "utils/prompt.h"

//=== DECLARATIONS =============================================================
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
int main(int argc, const char** argv);

/// @brief Sets up the mpc parser
///
/// This function sets up the parser using the mpc library by Daniel Holden. The
/// variables which contain the rules are documented itself.
void setup_parser(void);

/// @brief Cleans up the memory used by the interpreter.
///
/// This function cleans up the memory used by the parsers, as well as the
/// standard library if loaded, and the environment.
///
/// @param std A pointer to the standard-library (if loaded, else NULL)
/// @param env A pointer to the used environment
void cleanup(lval* std, lenv* env);

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
void cli_interpreter(lenv* env);

/// @brief Saves the input history to a file.
///
/// @parem num_elements The number of elements to append to the history file
void save_history(const int num_elements);

/// @brief Interprets a given file.
///
/// This function runs the file interpreter, the second option, next to the
/// command-line interpreter.
///
/// @param env The environment which we use for interpreting the file
/// @param file The file to interpret
void file_interpreter(lenv* env, const char* file);

/// @brief Loads the standard library and returns a pointer to it.
///
/// This function loads the standard library and returns a pointer to it. The
/// path to the stdlib is hardcoded.
///
/// @param env The environment into which the standard library should be loaded
/// @return A pointer to the standard library.
lval* get_stdlib(lenv* env);

/// @brief Sets up an environment which contains all builtins.
/// @return An environment which contains all builtin methods
lenv* set_env(void);

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

//=== MAIN METHOD ==============================================================
int main(int argc, const char** argv) {
    // Parse arguments and set up logfile, if necessary
    FILE* log_file = NULL;
    parse_args(argc, argv);
    if (0 == get_print_logs()) {
        log_file = prepare_logfile();
        log_add_fp(log_file, 0);
        log_set_quiet(true);
    }
    // Set up the interpreter
    setup_parser();
    lenv* env = set_env();
    lval* std = NULL;
    if (0 == get_nostdlib()) {
        std = get_stdlib(env);
    }
    if (NULL != get_filename()) {
        file_interpreter(env, get_filename());
    } else {
        cli_interpreter(env);
    }
    cleanup(std, env);
    return 0;
}

//=== HELPER METHODS ===========================================================
//--- Language Parser ----------------------------------------------------------
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

void cleanup(lval* std, lenv* env) {
    mpc_cleanup(8, number, symbol, sexpr, qexpr, string, comment, expr, lispy);
    if (NULL != std) {
        lval_del(std);
    }
    lenv_del(env);
}

mpc_parser_t* get_lispy_parser(void) {
    return lispy;
}

//--- Interpreter --------------------------------------------------------------
void cli_interpreter(lenv* env) {
    print_prompt();
    using_history();
    int i = 0;
    while (true) {
        char* input = readline(">>> ");
        add_history(input);
        i++;
        if (0 == strcmp(input, "exit")) {
            FREE(input);
            break;
        }
        mpc_result_t parse_result;
        if (mpc_parse("<stdin>", input, lispy, &parse_result)) {
            lval* read_result = lval_read(parse_result.output);
            assert(NULL != read_result);
            lval* eval_result = lval_eval(env, read_result);
            lval_println(eval_result);
            lval_del(eval_result);
            mpc_ast_delete(parse_result.output);
        } else {
            mpc_err_print(parse_result.error);
            mpc_err_delete(parse_result.error);
        }
        FREE(input);
    }
    save_history(i);
}

void file_interpreter(lenv* env, const char* file) {
    // Argument list with a single argument, the filename
    lval* argv_str = lval_str(file);
    lval* args = lval_add(lval_sexpr(), argv_str);

    // Pass to builtin load and get the result
    lval* x = builtin_load(env, args);

    // If the result is an error, be sure to print it
    if (LISPY_VAL_ERR == x->type) {
        lval_println(x);
    }
    lval_del(x);
}

lval* get_stdlib(lenv* env) {
    lval* standard =
        lval_add(lval_sexpr(), lval_str("/usr/local/lib/lispy/stdlib.lspy"));
    lval* std = builtin_load(env, standard);
    return std;
}

lenv* set_env(void) {
    lenv* env = lenv_new();
    lenv_add_builtins(env);
    return env;
}

void save_history(const int num_elements) {
    const char* cache_dir = getenv("XDG_CACHE_HOME");
    if (NULL == cache_dir) {
        return;
    }
    size_t history_file_size = strlen(cache_dir) + strlen("/lispy/history") + 1;
    char* history_file = malloc(history_file_size);
    strlcpy(history_file, cache_dir, history_file_size);
    strlcat(history_file, "/lispy/history", history_file_size);
    append_history(num_elements, history_file);
    return;
}
