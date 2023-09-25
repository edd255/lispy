#include "deps/mpc/mpc.h"
#include "io.h"

//=== WRITING ==================================================================

/* Print an lval */
void lval_print(lval* val) {
    assert(NULL != val);
    if (NULL == val) {
        return;
    }
    switch (val->type) {
        case LISPY_VAL_NUM: {
            printf("%li", val->num);
            break;
        }
        case LISPY_VAL_DEC: {
            printf("%f", val->dec);
            break;
        }
        case LISPY_VAL_ERR: {
            printf("Error: %s", val->err);
            break;
        }
        case LISPY_VAL_SYM: {
            printf("%s", val->sym);
            break;
        }
        case LISPY_VAL_FN: {
            if (val->builtin) {
                printf("<builtin>");
            } else {
                printf("(\\ ");
                lval_print(val->formals);
                putchar(' ');
                lval_print(val->body);
                putchar(')');
            }
            break;
        }
        case LISPY_VAL_STR: {
            lval_print_str(val);
            break;
        }
        case LISPY_VAL_SEXPR: {
            lval_print_expr(val, '(', ')');
            break;
        }
        case LISPY_VAL_QEXPR: {
            lval_print_expr(val, '{', '}');
            break;
        }
    }
}

void lval_print_expr(lval* val, char open, char close) {
    assert(NULL != val);
    putchar(open);
    for (int i = 0; i < val->count; i++) {
        // Print Value contained within
        lval_print(val->cell[i]);

        // Don't print trailing space if last element
        if (i != (val->count - 1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

/* Print a lval followed by a newline */
void lval_println(lval* val) {
    assert(NULL != val);
    lval_print(val);
    putchar('\n');
}

void lval_print_str(const lval* val) {
    assert(NULL != val);
    if (NULL == val) {
        return;
    }
    // Make a copy of the string
    char* escaped = MALLOC(val->len + 1);
    strlcpy(escaped, val->str, val->len + 1);

    // Pass it through the escape function
    escaped = mpcf_escape(escaped);

    // Print it between " characters
    printf("%s", escaped);

    // free copied string
    FREE(escaped);
}

char* ltype_name(enum LISPY_VAL type) {
    switch (type) {
        case LISPY_VAL_FN:
            return "Function";
        case LISPY_VAL_NUM:
            return "Number";
        case LISPY_VAL_DEC:
            return "Decimal";
        case LISPY_VAL_ERR:
            return "Error";
        case LISPY_VAL_SYM:
            return "Symbol";
        case LISPY_VAL_STR:
            return "String";
        case LISPY_VAL_SEXPR:
            return "S-Expression";
        case LISPY_VAL_QEXPR:
            return "Q-Expression";
        default:
            return "Unknown";
    }
}
