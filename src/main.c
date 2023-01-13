#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <editline/readline.h>
#include "../libs/mpc.h"

long eval_op(long x, char* op, long y)
{
        if (strcmp(op, "+") == 0) {
                return x + y;
        }
        if (strcmp(op, "-") == 0) {
                return x - y;
        }
        if (strcmp(op, "*") == 0) {
                return x * y;
        }
        if (strcmp(op, "/") == 0) {
                return x / y;
        }
        return 0;
}

long eval(mpc_ast_t* ast)
{
        if (strstr(ast -> tag, "number")) {
                return atoi(ast -> contents);
        }
        char* op = ast -> children[1] -> contents;
        long x = eval(ast -> children[2]);
        int i = 3;
        while (strstr(ast -> children[i] -> tag, "expr")) {
                x = eval_op(x, op, eval(ast -> children[i]));
                i++;
        }
        return x;
}

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
                if (strncmp(input, "exit", 4) == 0) {
                    return 0;
                }

                (void) add_history(input);

                mpc_result_t r;
                if (mpc_parse("<stdin>", input, clisp, &r)) {
                        mpc_ast_print(r.output);
                        long result = eval(r.output);
                        printf("%li\n", result);
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
