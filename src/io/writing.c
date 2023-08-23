#include "deps/mpc/mpc.h"
#include "io.h"

//=== WRITING ==================================================================

/* Print an lval_t */
void lval_print(lval_t* val) {
    assert(NULL != val);
    if (NULL == val) {
        return;
    }
    switch (val->type) {
        case LVAL_NUM: {
            printf("%li", val->num);
            break;
        }
        case LVAL_DEC: {
            printf("%f", val->dec);
            break;
        }
        case LVAL_ERR: {
            printf("Error: %s", val->err);
            break;
        }
        case LVAL_SYM: {
            printf("%s", val->sym);
            break;
        }
        case LVAL_FN: {
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
        case LVAL_STR: {
            lval_print_str(val);
            break;
        }
        case LVAL_SEXPR: {
            lval_print_expr(val, '(', ')');
            break;
        }
        case LVAL_QEXPR: {
            lval_print_expr(val, '{', '}');
            break;
        }
    }
}

void lval_print_expr(lval_t* val, char open, char close) {
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

/* Print a lval_t followed by a newline */
void lval_println(lval_t* val) {
    assert(NULL != val);

    lval_print(val);
    putchar('\n');
}

void lval_print_str(const lval_t* val) {
    assert(NULL != val);

    if (NULL == val) {
        return;
    }
    // Make a copy of the string
    char* escaped = MALLOC(strlen(val->str) + 1);
    strcpy(escaped, val->str);

    // Pass it through the escape function
    escaped = mpcf_escape(escaped);

    // Print it between " characters
    printf("%s", escaped);

    // free copied string
    FREE(escaped);
}

char* ltype_name(int type) {
    switch (type) {
        case LVAL_FN:
            return "Function";
        case LVAL_NUM:
            return "Number";
        case LVAL_DEC:
            return "Decimal";
        case LVAL_ERR:
            return "Error";
        case LVAL_SYM:
            return "Symbol";
        case LVAL_STR:
            return "String";
        case LVAL_SEXPR:
            return "S-Expression";
        case LVAL_QEXPR:
            return "Q-Expression";
        default:
            return "Unknown";
    }
}
