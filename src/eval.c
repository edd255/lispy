#include "eval.h"

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

