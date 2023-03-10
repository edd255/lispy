#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <editline/readline.h>
#include "../deps/mpc/mpc.h"
#include "lisp_value.h"
#include "eval.h"

int main(void)
{
        mpc_parser_t* number = mpc_new("number");
        mpc_parser_t* symbol = mpc_new("symbol");
        mpc_parser_t* s_expr = mpc_new("sexpr");
        mpc_parser_t* q_expr = mpc_new("qexpr");
        mpc_parser_t* expr   = mpc_new("expr");
        mpc_parser_t* clisp  = mpc_new("clisp");
        mpca_lang(
                MPCA_LANG_DEFAULT,
                "                                                     \
                number : /-?[0-9]+/ ;                                 \
                symbol : \"list\" | \"head\" | \"tail\"               \
                       | \"join\"| \"eval\" | '+' | '-' | '*' | '/' ; \
                sexpr  : '(' <expr>* ')' ;                            \
                qexpr  : '{' <expr>* '}' ;                            \
                expr   : <number> | <symbol> | <sexpr> | <qexpr> ;    \
                clisp  : /^/ <expr>* /$/ ;                            \
                ",
                number, symbol, s_expr, q_expr, expr, clisp
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
                        lisp_value_t* x = lisp_eval.eval(lisp_value.read(r.output));
                        lisp_value.println(x);
                        lisp_value.free(x);
                } else {
                        mpc_err_print(r.error);
                        mpc_err_delete(r.error);
                }

                free(input);
        }
        mpc_cleanup(6, number, symbol, s_expr, q_expr, expr, clisp);
        return 0;
}
