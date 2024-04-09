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
void cleanup(Value* std, Environment* env);

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
void cli_interpreter(Environment* env);

/// @brief Saves the input history to a file.
///
/// @parem num_elements The number of elements to append to the history file
void save_history(const int num_elements);

/// @brief Reads the history file.
int get_history(void);

/// @brief Interprets a given file.
///
/// This function runs the file interpreter, the second option, next to the
/// command-line interpreter.
///
/// @param env The environment which we use for interpreting the file
/// @param file The file to interpret
void file_interpreter(Environment* env, const char* file);

/// @brief Loads the standard library and returns a pointer to it.
///
/// This function loads the standard library and returns a pointer to it. The
/// path to the stdlib is hardcoded.
///
/// @param env The environment into which the standard library should be loaded
/// @return A pointer to the standard library.
Value* get_stdlib(Environment* env);

/// @brief Sets up an environment which contains all builtins.
/// @return An environment which contains all builtin methods
Environment* set_env(void);

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
    Environment* env = set_env();
    Value* std = NULL;
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

void cleanup(Value* std, Environment* env) {
    mpc_cleanup(8, number, symbol, sexpr, qexpr, string, comment, expr, lispy);
    if (NULL != std) {
        val_del(std);
    }
    env_del(env);
}

mpc_parser_t* get_lispy_parser(void) {
    return lispy;
}

//--- Interpreter --------------------------------------------------------------
void cli_interpreter(Environment* env) {
    print_banner();
    print_prompt();
    using_history();
    (void)get_history();
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
            Value* read_result = val_read(parse_result.output);
            assert(NULL != read_result);
            Value* eval_result = val_eval(env, read_result);
            val_println(eval_result);
            val_del(eval_result);
            mpc_ast_delete(parse_result.output);
        } else {
            mpc_err_print(parse_result.error);
            mpc_err_delete(parse_result.error);
        }
        FREE(input);
    }
    save_history(i);
}

void file_interpreter(Environment* env, const char* file) {
    // Argument list with a single argument, the filename
    Value* argv_str = val_str(file);
    Value* args = val_add(val_sexpr(), argv_str);
    // Pass to builtin load and get the result
    Value* x = builtin_load(env, args);
    // If the result is an error, be sure to print it
    if (LISPY_VAL_ERR == x->type) {
        val_println(x);
    }
    val_del(x);
}

Value* get_stdlib(Environment* env) {
    Value* standard =
        val_add(val_sexpr(), val_str("/usr/local/lib/lispy/stdlib.lspy"));
    Value* std = builtin_load(env, standard);
    return std;
}

Environment* set_env(void) {
    Environment* env = env_new();
    env_add_builtins(env);
    return env;
}

int get_history(void) {
    const char* cache_dir = getenv("XDG_CACHE_HOME");
    if (NULL == cache_dir) {
        return 1;
    }
    size_t history_file_size = strlen(cache_dir) + strlen("/lispy/history") + 1;
    char* history_file = malloc(history_file_size);
    strlcpy(history_file, cache_dir, history_file_size);
    strlcat(history_file, "/lispy/history", history_file_size);
    int result = read_history(history_file);
    free(history_file);
    return result;
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
    free(history_file);
    return;
}
