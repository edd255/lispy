//=== INCLUDES =================================================================

#include <assert.h>
#include "eval.h"


//=== DECLARATIONS =============================================================

static lisp_value_t* lval_eval_sexpr(lisp_value_t* value);
static lisp_value_t* lval_eval(lisp_value_t* value);
static lisp_value_t* builtin_op(lisp_value_t* a, char* op);
static lisp_value_t* builtin_head(lisp_value_t* a);
static lisp_value_t* builtin_tail(lisp_value_t* a);
static lisp_value_t* builtin_list(lisp_value_t* a);
static lisp_value_t* builtin_eval(lisp_value_t* a);
static lisp_value_t* builtin_join(lisp_value_t* a);
static lisp_value_t* builtin(lisp_value_t* a, char* func);


//=== DEFINITIONS ==============================================================

//--- BUILTIN OPERATORS --------------------------------------------------------

static lisp_value_t* builtin_op(lisp_value_t* a, char* op)
{
        assert(a != NULL);

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

static lisp_value_t* builtin_head(lisp_value_t* a)
{
        assert(a != NULL);

        // Check Error Condtions
        LISP_EVAL_ASSERT(
                a,
                a -> count == 1,
                "Function 'head' passed too many arguments!"
        );
        LISP_EVAL_ASSERT(
                a,
                a -> cell[0] -> type == LISP_VALUE_QEXPR,
                "Function 'head' passed incorrect type!"
        );
        LISP_EVAL_ASSERT(
                a,
                a -> cell[0] -> count != 0,
                "Function 'head' passed {}!"
        );

        // Otherwise, take the first argument
        lisp_value_t* value = lisp_value.take(a, 0);

        // Delete all elementst that are not head and return
        while (value -> count > 1) {
                lisp_value.free(lisp_value.pop(value, 1));
        }
        return value;
}

static lisp_value_t* builtin_tail(lisp_value_t* a)
{
        assert(a != NULL);

        // Check Error Conditions
        LISP_EVAL_ASSERT(
                a,
                a -> count == 1,
                "Function 'tail' passed too many arguments!"
        );
        LISP_EVAL_ASSERT(
                a,
                a -> cell[0] -> type == LISP_VALUE_QEXPR,
                "Function 'tail' passed incorrect type!"
        );
        LISP_EVAL_ASSERT(
                a,
                a -> cell[0] -> count != 0,
                "Function 'tail' passed {}!"
        );

        // Take the first argument
        lisp_value_t* value = lisp_value.take(a, 0);

        // Delete the first element and return
        lisp_value.free(lisp_value.pop(value, 0));
        return value;
}

lisp_value_t* builtin_list(lisp_value_t* a)
{
        assert(a != NULL);

        a -> type = LISP_VALUE_QEXPR;
        return a;
}

lisp_value_t* builtin_eval(lisp_value_t* a)
{
        assert(a != NULL);

        LISP_EVAL_ASSERT(
                a,
                a -> count == 1,
                "Function 'eval' passed too many arguments!"
        );
        LISP_EVAL_ASSERT(
                a,
                a -> cell[0] -> type == LISP_VALUE_QEXPR,
                "Function 'eval' passed incorrect type!"
        );
        lisp_value_t* x = lisp_value.take(a, 0);
        x -> type = LISP_VALUE_SEXPR;
        return lval_eval(x);
}

lisp_value_t* lval_join(lisp_value_t* x, lisp_value_t* y)
{
        assert(x != NULL);
        assert(y != NULL);

        // For each cell in 'y' add it to 'x'
        while (y -> count) {
                x = lisp_value.add(x, lisp_value.pop(y, 0));
        }

        // Delete the empty 'y' and return 'x'
        lisp_value.free(y);
        return x;
}

static lisp_value_t* builtin_join(lisp_value_t* a)
{
        assert(a != NULL);

        for (int i = 0; i < a -> count; i++) {
                LISP_EVAL_ASSERT(
                        a,
                        a -> cell[i] -> type == LISP_VALUE_QEXPR,
                        "Function 'join' passed incorrect type."
                );
        }
        lisp_value_t* x = lisp_value.pop(a, 0);
        while (a -> count) {
                x = lval_join(x, lisp_value.pop(a, 0));
        }
        lisp_value.free(a);
        return x;
}

static lisp_value_t* builtin(lisp_value_t* a, char* func)
{
        assert(a != NULL);

        if (strcmp("list", func) == 0) {
                return builtin_list(a);
        }
        if (strcmp("head", func) == 0) {
                return builtin_head(a);
        }
        if (strcmp("tail", func) == 0) {
                return builtin_tail(a);
        }
        if (strcmp("join", func) == 0) {
                return builtin_join(a);
        }
        if (strcmp("eval", func) == 0) {
                return builtin_eval(a);
        }
        if (strcmp("+-/*", func) == 0) {
                return builtin_op(a, func);
        }
        lisp_value.free(a);
        return lisp_value.error("Unknown function!");
}


//--- EVALUATION METHODS -------------------------------------------------------

static lisp_value_t* lval_eval_sexpr(lisp_value_t* value)
{
        assert(value != NULL);

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
                return lisp_value.error(
                        "S-Expression does not start with a symbol."
                );
        }

        // Call builtin with operator
        lisp_value_t* result = builtin(value, first -> symbol);
        lisp_value.free(first);
        return result;
}

static lisp_value_t* lval_eval(lisp_value_t* value)
{
        assert(value != NULL);

        if (value -> type == LISP_VALUE_SEXPR) {
                return lval_eval_sexpr(value);
        }
        return value;
}


//=== EXPORTING NAMESPACE ======================================================

namespace_lisp_eval const lisp_eval = {
        lval_eval_sexpr,
        lval_eval,
        builtin_op,
        builtin_head,
        builtin_tail,
        builtin_list,
        builtin_eval,
        builtin_join,
        builtin
};
