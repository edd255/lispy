#include "eval.h"

lisp_value_t eval_op(lisp_value_t x, char* op, lisp_value_t y)
{
        if (x.type == LISP_VAL_ERR) {
                return x;
        }
        if (y.type == LISP_VAL_ERR) {
                return y;
        }
        if (strcmp(op, "+") == 0) {
                return lisp_val_num(x.number + y.number); 
        }
        if (strcmp(op, "-") == 0) {
                return lisp_val_num(x.number - y.number);
        }
        if (strcmp(op, "*") == 0) {
                return lisp_val_num(x.number * y.number);
        }
        if (strcmp(op, "/") == 0) {
                return y.number == 0
                        ? lisp_val_err(ERR_DIV_ZERO)
                        : lisp_val_num(x.number / y.number);
        }
        return lisp_val_err(ERR_BAD_OP);
}

lisp_value_t eval(mpc_ast_t* ast)
{
        if (strstr(ast -> tag, "number")) {
                errno = 0;
                long x = strtol(ast -> contents, NULL, 10);
                return errno != ERANGE
                        ? lisp_val_num(x)
                        : lisp_val_err(ERR_BAD_NUM);
        }
        char* op = ast -> children[1] -> contents;
        lisp_value_t x = eval(ast -> children[2]);
        int i = 3;
        while (strstr(ast -> children[i] -> tag, "expr")) {
                x = eval_op(x, op, eval(ast -> children[i]));
                i++;
        }
        return x;
}
