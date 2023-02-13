#include "lisp_value.h"

/* Create a new number type lval */
lisp_value_t lisp_val_num(long x) {
        lisp_value_t v;
        v.type = LISP_VAL_NUM;
        v.number = x;
        return v;
}

/* Create a new error type lval */
lisp_value_t lisp_val_err(int x) {
        lisp_value_t v;
        v.type = LISP_VAL_ERR;
        v.error = x;
        return v;
}

void lisp_val_print(lisp_value_t value)
{
        switch (value.type) {
                case LISP_VAL_NUM: {
                        printf("%li", value.number);
                        break;
                }
                case LISP_VAL_ERR: {
                        if (value.error == ERR_DIV_ZERO) {
                                printf("Error: Division by Zero");
                        } else if (value.error == ERR_BAD_OP) {
                                printf("Error: Invalid operator");
                        } else if (value.error == ERR_BAD_NUM) {
                                printf("Error: Invalid number");
                        }
                }
                        break;
        }
}

void lisp_val_println(lisp_value_t value)
{
        lisp_val_print(value);
        printf("\n");
}
