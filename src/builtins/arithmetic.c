#include "builtins/arithmetic.h"

#include <math.h>

#include "builtins/helpers.h"
#include "io.h"

//==== Arithmetic functions ====================================================
lval_t* builtin_op(lenv_t* env, lval_t* args, char* op) {
    assert(NULL != env);
    assert(NULL != args);
    assert(NULL != op);
    UNUSED(env);
    LCHECK_TYPES(op, args, 0, LVAL_NUM, LVAL_DEC);

    // Ensure all arguments are numbers
    for (int i = 0; i < args->count; i++) {
        LCHECK_TYPE(op, args, i, args->cell[0]->type);
    }

    // Pop the first element
    lval_t* x = lval_pop(args, 0);
    assert(NULL != x);

    // If no arguments and sub then perform unary negation
    if ((0 == strcmp(op, "-")) && 0 == args->count) {
        switch (x->type) {
            case LVAL_NUM: {
                x->num = -x->num;
                break;
            }
            case LVAL_DEC: {
                x->dec = -x->dec;
                break;
            }
        }
    }

    // While there are still elements remaining
    while (args->count > 0) {
        // Pop the next element
        lval_t* y = lval_pop(args, 0);
        assert(NULL != y);
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
                        if (0 == y->num) {
                            lval_del(x);
                            lval_del(y);
                            x = lval_err("Division By Zero!");
                            break;
                        }
                        x->num /= y->num;
                        break;
                    }
                    case LVAL_DEC: {
                        if (0.0 == y->dec) {
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
                        if (0 == y->num) {
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
                lval_del(y);
                break;
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
                lval_del(y);
                break;
            }
            case LOP_UNKNOWN: {
                lval_del(y);
                break;
            }
        }
    }
    lval_del(args);
    return x;
}

lval_t* builtin_add(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_op(env, args, "+");
}

lval_t* builtin_sub(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_op(env, args, "-");
}

lval_t* builtin_mul(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_op(env, args, "*");
}

lval_t* builtin_div(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_op(env, args, "/");
}

lval_t* builtin_mod(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_op(env, args, "%");
}

lval_t* builtin_pow(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_op(env, args, "^");
}

lval_t* builtin_max(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_op(env, args, "max");
}

lval_t* builtin_min(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    return builtin_op(env, args, "min");
}
