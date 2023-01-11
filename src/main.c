#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <editline/readline.h>
#include "../libs/mpc.h"

int main()
{
        mpc_parser_t* number   = mpc_new("number");
        mpc_parser_t* operator = mpc_new("operator");
        mpc_parser_t* expr     = mpc_new("expr");
        mpc_parser_t* clisp    = mpc_new("clisp");

        mpca_lang(
                MPCA_LANG_DEFAULT,
                "                                                       \
                number   : /-?[0-9]+/ ;                                 \
                operator : '+' | '-' | '*' | '/' ;                      \
                expr     : <number> | '(' <operator> <expr>+ ')' ;      \
                clisp    : /^/ <operator> <expr>+ /$/ ;                 \
                ",
                number, operator, expr, clisp
        );

        printf("CLisp 0.1\n");
        printf("Press Ctrcl+c to exit.\n");

        while (true) {
                char* input = readline("> ");
                (void) add_history(input);

                mpc_result_t r;
                if (mpc_parse("<stdin>", input, clisp, &r)) {
                        mpc_ast_print(r.output);
                        mpc_ast_delete(r.output);
                } else {
                        mpc_err_print(r.error);
                        mpc_err_delete(r.error);
                }

                free(input);
        }

        mpc_cleanup(4, number, operator, expr, clisp);
        return 0;
}
