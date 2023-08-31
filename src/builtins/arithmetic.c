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
    LCHECK_TYPES(op, args, 0, LISPY_VAL_NUM, LISPY_VAL_DEC);

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
            case LISPY_VAL_NUM: {
                x->num = -x->num;
                break;
            }
            case LISPY_VAL_DEC: {
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
        switch (arithm_op_from_str(op)) {
            case LISPY_ARITHM_ADD: {
                switch (x->type) {
                    case LISPY_VAL_NUM: {
                        x->num = __builtin_elementwise_add_sat(x->num, y->num);
                        break;
                    }
                    case LISPY_VAL_DEC: {
                        x->dec += y->dec;
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LISPY_ARITHM_SUB: {
                switch (x->type) {
                    case LISPY_VAL_NUM: {
                        x->num = __builtin_elementwise_sub_sat(x->num, y->num);
                        break;
                    }
                    case LISPY_VAL_DEC: {
                        x->dec -= y->dec;
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LISPY_ARITHM_MUL: {
                switch (x->type) {
                    case LISPY_VAL_NUM: {
                        x->num *= y->num;
                        break;
                    }
                    case LISPY_VAL_DEC: {
                        x->dec *= y->dec;
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LISPY_ARITHM_DIV: {
                switch (x->type) {
                    case LISPY_VAL_NUM: {
                        if (0 == y->num) {
                            lval_del(x);
                            lval_del(y);
                            x = lval_err("Division By Zero!");
                            break;
                        }
                        x->num /= y->num;
                        lval_del(y);
                        break;
                    }
                    case LISPY_VAL_DEC: {
                        if (0.0 == y->dec) {
                            lval_del(x);
                            lval_del(y);
                            x = lval_err("Division By Zero!");
                            break;
                        }
                        x->dec /= y->dec;
                        lval_del(y);
                        break;
                    }
                }
                break;
            }
            case LISPY_ARITHM_MOD: {
                switch (x->type) {
                    case LISPY_VAL_NUM: {
                        if (0 == y->num) {
                            lval_del(x);
                            lval_del(y);
                            x = lval_err("Division By Zero!");
                            break;
                        }
                        x->num %= y->num;
                        lval_del(y);
                        break;
                    }
                    case LISPY_VAL_DEC: {
                        lval_del(x);
                        lval_del(y);
                        x = lval_err("Modulo not allowed for decimals!");
                        break;
                    }
                }
                break;
            }
            case LISPY_ARITHM_POW: {
                switch (x->type) {
                    case LISPY_VAL_NUM: {
                        x->num = power_long(x->num, y->num);
                        break;
                    }
                    case LISPY_VAL_DEC: {
                        x->dec = pow(x->dec, y->dec);
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LISPY_ARITHM_MAX: {
                switch (x->type) {
                    case LISPY_VAL_NUM: {
                        x->num = __builtin_elementwise_max(x->num, y->num);
                        break;
                    }
                    case LISPY_VAL_DEC: {
                        if (x->dec <= y->dec) {
                            x->dec = y->dec;
                        }
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LISPY_ARITHM_MIN: {
                switch (x->type) {
                    case LISPY_VAL_NUM: {
                        x->num = __builtin_elementwise_min(x->num, y->num);
                        break;
                    }
                    case LISPY_VAL_DEC: {
                        if (x->dec >= y->dec) {
                            x->dec = y->dec;
                        }
                        break;
                    }
                }
                lval_del(y);
                break;
            }
            case LISPY_ARITHM_UNKNOWN: {
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

lval_t* builtin_sum(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);

    lval_t* list = args->cell[0];
    float dec_sum = 0;
    long int_sum = 0;
    bool dec_flag = false;
    for (int i = 0; i < list->count; i++) {
        LCHECK_TYPES(__func__, list, i, LISPY_VAL_NUM, LISPY_VAL_DEC);
        switch (list->cell[i]->type) {
            case LISPY_VAL_NUM: {
                dec_sum += (float)list->cell[i]->num;
                int_sum += list->cell[i]->num;
                break;
            }
            case LISPY_VAL_DEC: {
                dec_sum += list->cell[i]->dec;
                dec_flag = true;
                break;
            }
        }
    }
    lval_del(args);
    return dec_flag ? lval_dec(dec_sum) : lval_num(int_sum);
}

lval_t* builtin_prod(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);

    lval_t* list = args->cell[0];
    float dec_prod = 1;
    long int_prod = 1;
    bool dec_flag = false;
    for (int i = 0; i < list->count; i++) {
        LCHECK_TYPES(__func__, list, i, LISPY_VAL_NUM, LISPY_VAL_DEC);
        switch (list->cell[i]->type) {
            case LISPY_VAL_NUM: {
                dec_prod *= (float)list->cell[i]->num;
                int_prod *= list->cell[i]->num;
                break;
            }
            case LISPY_VAL_DEC: {
                dec_prod *= list->cell[i]->dec;
                dec_flag = true;
                break;
            }
        }
    }
    lval_del(args);
    return dec_flag ? lval_dec(dec_prod) : lval_num(int_prod);
}
