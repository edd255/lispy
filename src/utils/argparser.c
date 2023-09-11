#include "utils/argparser.h"

#include <stdlib.h>

#include "deps/argparse/argparse.h"

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

//--- Command-Line Argument Parser ---------------------------------------------
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

char* get_filename(void) {
    return filename;
}

int get_nostdlib(void) {
    return no_stdlib;
}

int get_print_logs(void) {
    return print_logs;
}
