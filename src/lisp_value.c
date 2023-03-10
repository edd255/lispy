//=== INCLUDES =================================================================

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "lisp_value.h"


//=== DECLARATIONS =============================================================

static lisp_value_t* lval_num(long x);
static lisp_value_t* lval_err(char* m);
static lisp_value_t* lval_sym(char* s);
static lisp_value_t* lval_sexpr(void);
static lisp_value_t* lval_qexpr(void);
static void lval_print(lisp_value_t* value);
static void lval_println(lisp_value_t* value);
static void lval_free(lisp_value_t* value);
static lisp_value_t* lval_read_num(mpc_ast_t* tree);
static lisp_value_t* lval_read(mpc_ast_t* tree);
static lisp_value_t* lval_add(lisp_value_t* value, lisp_value_t* x);
static void lval_expr_print(lisp_value_t* value, char open, char close);
static lisp_value_t* lval_pop(lisp_value_t* value, int i);
static lisp_value_t* lval_take(lisp_value_t* value, int i);


//=== DEFINITIONS ==============================================================

//--- Creating expressions -----------------------------------------------------

/* Create a pointer to a new number type lval */
static lisp_value_t* lval_num(long number)
{
        lisp_value_t* value = malloc(sizeof(lisp_value_t));
        value -> type = LISP_VALUE_NUMBER;
        value -> number = number;
        return value;
}

/* Create a pointer to a new error type lval */
static lisp_value_t* lval_err(char* message)
{
        lisp_value_t* value = malloc(sizeof(lisp_value_t));
        value -> type = LISP_VALUE_ERROR;
        value -> error = malloc(strlen(message) + 1);
        strcpy(value -> error, message);
        return value;
}

/* Construct a pointer to a new symbol lval */
static lisp_value_t* lval_sym(char* symbol)
{
        lisp_value_t* value = malloc(sizeof(lisp_value_t));
        value -> type = LISP_VALUE_SYMBOL;
        value -> symbol = malloc(strlen(symbol) + 1);
        strcpy(value -> symbol, symbol);
        return value;
}

/* Construct a pointer to a new empty sexpr lval */
static lisp_value_t* lval_sexpr(void)
{
        lisp_value_t* value = malloc(sizeof(lisp_value_t));
        value -> type = LISP_VALUE_SEXPR;
        value -> count = 0;
        value -> cell = NULL;
        return value;
}

static lisp_value_t* lval_qexpr(void)
{
        lisp_value_t* value = malloc(sizeof(lisp_value_t));
        value -> type = LISP_VALUE_QEXPR;
        value -> count = 0;
        value -> cell = NULL;
        return value;
}


//--- Printing expressions -----------------------------------------------------

/* Print out the value */
static void lval_print(lisp_value_t* value)
{
        assert(value != NULL);

        switch (value -> type) {
                case LISP_VALUE_NUMBER: printf("%li", value -> number); break;
                case LISP_VALUE_ERROR:  printf("Error: %s", value -> error); break;
                case LISP_VALUE_SYMBOL: printf("%s", value -> symbol); break;
                case LISP_VALUE_SEXPR:  lval_expr_print(value, '(', ')'); break;
                case LISP_VALUE_QEXPR:  lval_expr_print(value, '{', '}'); break;
        }
}

/* Print out the value with newline */
static void lval_println(lisp_value_t* value)
{
        assert(value != NULL);

        lval_print(value);
        printf("\n");
}

/* Print an expression */
static void lval_expr_print(lisp_value_t* value, char open, char close)
{
        assert(value != NULL);

        putchar(open);
        for (int i = 0; i < value -> count; i++) {

                // Print the value contained within
                lval_print(value -> cell[i]);

                // Don't print a trailing space
                if (i != (value -> count - 1)) {
                        putchar(' ');
                }
        }
        putchar(close);
}

//--- Freeing expressions ------------------------------------------------------

/* Free the memory of the value */
static void lval_free(lisp_value_t* value)
{
        assert(value != NULL);

        switch (value -> type) {
                // Number types don't need special treatment
                case LISP_VALUE_NUMBER: break;

                // Free the strings for errors and symbols
                case LISP_VALUE_ERROR:  free(value -> error); break;
                case LISP_VALUE_SYMBOL: free(value -> symbol); break;

                // Free all elements inside S-expressions and Q-expressions
                case LISP_VALUE_QEXPR:
                case LISP_VALUE_SEXPR: {
                        for (int i = 0; i < value -> count; i++) {
                                lval_free(value -> cell[i]);
                        }

                        // Free the memory that contains the pointers
                        free(value -> cell);
                        break;
                }
        }

        // Free the expression itself
        free(value);
}

//--- Reading expressions ------------------------------------------------------

/* Read a number from a tree */
static lisp_value_t* lval_read_num(mpc_ast_t* tree)
{
        assert(tree != NULL);

        errno = 0;
        long x = strtol(tree -> contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err("Invalid number");
}

/* Read a value from a tree */
static lisp_value_t* lval_read(mpc_ast_t* tree)
{
        assert(tree != NULL);

        // If symbol or number return conversion to that type
        if (strstr(tree -> tag, "number")) {
                return lval_read_num(tree);
        }
        if (strstr(tree -> tag, "symbol")) {
                return lval_sym(tree -> contents);
        }

        // If root (>) or sexpr then create an empty list
        lisp_value_t* x = NULL;
        if (strcmp(tree -> tag, ">") == 0) {
                x = lval_sexpr();
        }
        if (strstr(tree -> tag, "sexpr")) {
                x = lval_sexpr();
        }
        if (strstr(tree -> tag, "qexpr")) {
                x = lval_qexpr();
        }

        // Fill this list with any valid expression contained within
        for (int i = 0; i < tree -> children_num; i++) {
                if (strcmp(tree -> children[i] -> contents, "(") == 0) {
                        continue;
                }
                if (strcmp(tree -> children[i] -> contents, ")") == 0) {
                        continue;
                }
                if (strcmp(tree -> children[i] -> contents, "}") == 0) {
                        continue;
                }
                if (strcmp(tree -> children[i] -> contents, "{") == 0) {
                        continue;
                }
                if (strcmp(tree -> children[i] -> tag,  "regex") == 0) {
                        continue;
                }
                x = lval_add(x, lval_read(tree -> children[i]));
        }
        return x;
}

/* Add an element to an S-expression */
static lisp_value_t* lval_add(lisp_value_t* value, lisp_value_t* x)
{
        assert(value != NULL);
        assert(x != NULL);

        value -> count++;
        value -> cell = realloc(
                value -> cell,
                sizeof(lisp_value_t*) * value -> count
        );
        value -> cell[value -> count - 1] = x;
        return value;
}


//--- MANIPULATING S-EXPRESSIONS ----------------------------------------------

/* Extract a single element from an S-Expression at index i */
static lisp_value_t* lval_pop(lisp_value_t* value, int i)
{
        assert(value != NULL);

        // Find the element at index i
        lisp_value_t* x = value -> cell[i];

        // Shift memory after the item at i over the top
        memmove(
                &value -> cell[i],
                &value -> cell[i + 1],
                sizeof(lisp_value_t*) * (value -> count - i - 1)
        );

        // Decrease the count of items in the list
        value -> count--;

        // Reallocate the memory used
        value -> cell = realloc(
                value -> cell,
                sizeof(lisp_value_t*) * value -> count
        );
        return x;
}

/* Deletes the list it has extracted the element at index i from */
static lisp_value_t* lval_take(lisp_value_t* value, int i)
{
        assert(value != NULL);
        lisp_value_t* x = lval_pop(value, i);
        lval_free(value);
        return x;
}


//=== EXPORTING NAMESPACE ======================================================

namespace_lisp_value const lisp_value = {
        lval_num,
        lval_err,
        lval_sym,
        lval_sexpr,
        lval_qexpr,
        lval_print,
        lval_println,
        lval_free,
        lval_read_num,
        lval_read,
        lval_add,
        lval_pop,
        lval_take
};
