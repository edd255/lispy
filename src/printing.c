#include "printing.h"

//=== PRINTING =================================================================

/* Print an lval_t */
void lval_print(lval_t* v) {
    assert(v != NULL);

    if (v == NULL) {
        return;
    }
    switch (v->type) {
        case LVAL_NUM: {
            printf("%li", v->num);
            break;
        }
        case LVAL_DEC: {
            printf("%f", v->dec);
            break;
        }
        case LVAL_ERR: {
            printf("Error: %s", v->err);
            break;
        }
        case LVAL_SYM: {
            printf("%s", v->sym);
            break;
        }
        case LVAL_FN: {
            if (v->builtin) {
                printf("<builtin>");
            } else {
                printf("(\\ ");
                lval_print(v->formals);
                putchar(' ');
                lval_print(v->body);
                putchar(')');
            }
            break;
        }
        case LVAL_STR: {
            lval_print_str(v);
            break;
        }
        case LVAL_SEXPR: {
            lval_print_expr(v, '(', ')');
            break;
        }
        case LVAL_QEXPR: {
            lval_print_expr(v, '{', '}');
            break;
        }
    }
}

void lval_print_expr(lval_t* v, char open, char close) {
    assert(v != NULL);

    putchar(open);
    for (int i = 0; i < v->count; i++) {
        // Print Value contained within
        lval_print(v->cell[i]);

        // Don't print trailing space if last element
        if (i != (v->count - 1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

/* Print a lval_t followed by a newline */
void lval_println(lval_t* v) {
    assert(v != NULL);

    lval_print(v);
    putchar('\n');
}

void lval_print_str(lval_t* v) {
    assert(v != NULL);

    if (v == NULL) {
        return;
    }
    // Make a copy of the string
    char* escaped = malloc(strlen(v->str) + 1);
    strcpy(escaped, v->str);

    // Pass it through the escape function
    escaped = mpcf_escape(escaped);

    // Print it between " characters
    printf("\"%s\"", escaped);

    // free copied string
    free(escaped);
}

char* ltype_name(int t) {
    switch (t) {
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
