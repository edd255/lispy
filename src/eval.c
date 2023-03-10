//=== INCLUDES =================================================================

#include "eval.h"


//=== DECLARATIONS =============================================================

static lisp_value_t* lval_eval_sexpr(lisp_value_t* value);
static lisp_value_t* lval_eval(lisp_value_t* value);


//=== DEFINITIONS ==============================================================

lisp_value_t* builtin_op(lisp_value_t* a, char* op)
{
        // Ensure all arguments are numbers
        for (int i = 0; i < a -> count; i++) {
                if (a -> cell[i] -> type != LISP_VALUE_NUMBER) {
                        lisp_value.free(a);
                        return lisp_value.error("Cannot operate on non-number");
                }
        }

        // Pop the first element
        lisp_value_t* x = lisp_value.pop(a, 0);

        // If no arguments and sub then perform unary negation
        if ((strcmp(op, "-") == 0) && a -> count == 0) {
                x -> number = -x -> number;
        }

        // While there are still elements remaining
        while (a -> count > 0) {
                lisp_value_t* y = lisp_value.pop(a, 0);
                if (strcmp(op, "+") == 0) {
                        x -> number += y -> number;
                }
                if (strcmp(op, "-") == 0) {
                        x -> number -= y -> number;
                }
                if (strcmp(op, "*") == 0) {
                        x -> number *= y -> number;
                }
                if (strcmp(op, "/") == 0) {
                        if (y -> number == 0) {
                                lisp_value.free(x);
                                lisp_value.free(y);
                                x = lisp_value.error("Division by Zero!");
                                break;
                        }
                        x -> number /= y -> number;
                }
                lisp_value.free(y);
        }
        lisp_value.free(a);
        return x;
}


static lisp_value_t* lval_eval_sexpr(lisp_value_t* value)
{
        // Evaluate Children
        for (int i = 0; i < value -> count; i++) {
                value -> cell[i] = lval_eval(value -> cell[i]);
        }

        // Error Checking
        for (int i = 0; i < value -> count; i++) {
                if (value -> cell[i] -> type == LISP_VALUE_ERROR) {
                        return lisp_value.take(value, i);
                }
        }

        // Empty Expression
        if (value -> count == 0) {
                return value;
        }

        // Single Expression
        if (value -> count == 1) {
                return lisp_value.take(value, 0);
        }

        // Ensure the first element is a symbol
        lisp_value_t* first = lisp_value.pop(value, 0);
        if (first -> type != LISP_VALUE_SYMBOL) {
                lisp_value.free(first);
                lisp_value.free(value);
                return lisp_value.error("S-Expression does not start with a symbol.");
        }

        // Call builtin with operator
        lisp_value_t* result = builtin_op(value, first -> symbol);
        lisp_value.free(first);
        return result;
}

static lisp_value_t* lval_eval(lisp_value_t* value)
{
        if (value -> type == LISP_VALUE_SEXPR) {
                return lval_eval_sexpr(value);
        }
        return value;
}

//=== EXPORTING NAMESPACE ======================================================

namespace_lisp_eval const lisp_eval = {
        lval_eval_sexpr,
        lval_eval
};
