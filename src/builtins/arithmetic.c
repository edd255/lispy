#include "arithmetic.h"

#include "../printing.h"
#include "../values.h"
#include "helpers.h"

//==== Arithmetic functions ====================================================
lval_t* builtin_op(lenv_t* e, lval_t* a, char* op) {
    assert(e != NULL);
    assert(a != NULL);
    assert(op != NULL);
    UNUSED(e);
    LASSERT_TYPES(op, a, 0, LVAL_NUM, LVAL_DEC)

    // Ensure all arguments are numbers
    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE(op, a, i, a->cell[0]->type)
    }

    // Pop the first element
    lval_t* x = lval_pop(a, 0);
    assert(x != NULL);

    // If no arguments and sub then perform unary negation
    if ((strcmp(op, "-") == 0) && a->count == 0) {
        switch (x->type) {
            case LVAL_NUM: {
                x->num = -x->num;
            }
            case LVAL_DEC: {
                x->dec = -x->dec;
            }
        }
    }

    // While there are still elements remaining
    while (a->count > 0) {
        // Pop the next element
        lval_t* y = lval_pop(a, 0);
        assert(y != NULL);
        switch (op_from_string(op)) {
            case LOP_ADD: {
                switch (x->type) {
                    case LVAL_NUM: {
                        x->num = __builtin_elementwise_add_sat(x->num, y->num);
                        break;
                    }
                    case LVAL_DEC: {
                        x->dec += y->dec;
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LOP_SUB: {
                switch (x->type) {
                    case LVAL_NUM: {
                        x->num = __builtin_elementwise_sub_sat(x->num, y->num);
                        break;
                    }
                    case LVAL_DEC: {
                        x->dec -= y->dec;
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LOP_MUL: {
                switch (x->type) {
                    case LVAL_NUM: {
                        x->num *= y->num;
                        break;
                    }
                    case LVAL_DEC: {
                        x->dec *= y->dec;
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LOP_DIV: {
                switch (x->type) {
                    case LVAL_NUM: {
                        if (y->num == 0) {
                            lval_del(x);
                            lval_del(y);
                            x = lval_err("Division By Zero!");
                            break;
                        }
                        x->num /= y->num;
                        break;
                    }
                    case LVAL_DEC: {
                        if (y->dec == 0.0) {
                            lval_del(x);
                            lval_del(y);
                            x = lval_err("Division By Zero!");
                            break;
                        }
                        x->dec /= y->dec;
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LOP_MOD: {
                switch (x->type) {
                    case LVAL_NUM: {
                        if (y->num == 0) {
                            lval_del(x);
                            lval_del(y);
                            x = lval_err("Division By Zero!");
                            break;
                        }
                        x->num %= y->num;
                        break;
                    }
                    case LVAL_DEC: {
                        lval_del(x);
                        lval_del(y);
                        x = lval_err("Modulo not allowed for decimals!");
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LOP_POW: {
                switch (x->type) {
                    case LVAL_NUM: {
                        x->num = power_long(x->num, y->num);
                        break;
                    }
                    case LVAL_DEC: {
                        x->dec = pow(x->dec, y->dec);
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LOP_MAX: {
                switch (x->type) {
                    case LVAL_NUM: {
                        x->num = __builtin_elementwise_max(x->num, y->num);
                        break;
                    }
                    case LVAL_DEC: {
                        if (x->dec <= y->dec) {
                            x->dec = y->dec;
                        }
                        break;
                    }
                }
            }
            case LOP_MIN: {
                switch (x->type) {
                    case LVAL_NUM: {
                        x->num = __builtin_elementwise_min(x->num, y->num);
                        break;
                    }
                    case LVAL_DEC: {
                        if (x->dec >= y->dec) {
                            x->dec = y->dec;
                        }
                        break;
                    }
                }
            }
            case LOP_UNKNOWN: {
                lval_del(y);
                break;
            }
        }
    }
    lval_del(a);
    return x;
}

lval_t* builtin_add(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_op(e, a, "+");
}

lval_t* builtin_sub(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_op(e, a, "-");
}

lval_t* builtin_mul(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_op(e, a, "*");
}

lval_t* builtin_div(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_op(e, a, "/");
}

lval_t* builtin_mod(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_op(e, a, "%");
}

lval_t* builtin_pow(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_op(e, a, "^");
}

lval_t* builtin_max(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_op(e, a, "max");
}

lval_t* builtin_min(lenv_t* e, lval_t* a) {
    assert(e != NULL);
    assert(a != NULL);

    return builtin_op(e, a, "min");
}
