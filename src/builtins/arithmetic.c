#include "builtins/arithmetic.h"

#include <math.h>

#include "builtins/helpers.h"
#include "io.h"

//==== Arithmetic functions ====================================================
Value* builtin_op(Environment* env, Value* args, char* op) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    ASSERT(op != NULL);
    UNUSED(env);
    LCHECK_TYPES(op, args, 0, LISPY_VAL_NUM, LISPY_VAL_DEC);
    // Ensure all arguments are numbers
    for (int i = 0; i < args->count; i++) {
        LCHECK_TYPE(op, args, i, args->cell[0]->type);
    }
    // Pop the first element
    Value* x = val_pop(args, 0);
    ASSERT(x != NULL);
    // If no arguments and sub then perform unary negation
    if ((strcmp(op, "-") == 0) && args->count == 0) {
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
        Value* y = val_pop(args, 0);
        ASSERT(y != NULL);
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
                val_del(y);
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
                val_del(y);
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
                val_del(y);
                break;
            }
            case LISPY_ARITHM_DIV: {
                switch (x->type) {
                    case LISPY_VAL_NUM: {
                        if (y->num == 0) {
                            val_del(x);
                            val_del(y);
                            x = val_err("Division By Zero!");
                            break;
                        }
                        x->num /= y->num;
                        val_del(y);
                        break;
                    }
                    case LISPY_VAL_DEC: {
                        if (y->dec == 0.0) {
                            val_del(x);
                            val_del(y);
                            x = val_err("Division By Zero!");
                            break;
                        }
                        x->dec /= y->dec;
                        val_del(y);
                        break;
                    }
                }
                break;
            }
            case LISPY_ARITHM_MOD: {
                switch (x->type) {
                    case LISPY_VAL_NUM: {
                        if (y->num == 0) {
                            val_del(x);
                            val_del(y);
                            x = val_err("Division By Zero!");
                            break;
                        }
                        x->num %= y->num;
                        val_del(y);
                        break;
                    }
                    case LISPY_VAL_DEC: {
                        val_del(x);
                        val_del(y);
                        x = val_err("Modulo not allowed for decimals!");
                        break;
                    }
                }
                break;
            }
            case LISPY_ARITHM_POW: {
                switch (x->type) {
                    case LISPY_VAL_NUM: {
                        if (y->num >= 0) {
                            x->num = power_long(x->num, y->num);
                        } else {
                            x->type = LISPY_VAL_DEC;
                            x->dec = pow(x->num, y->num);
                        }
                        break;
                    }
                    case LISPY_VAL_DEC: {
                        x->dec = pow(x->dec, y->dec);
                        break;
                    }
                }
                val_del(y);
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
                val_del(y);
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
                val_del(y);
                break;
            }
            case LISPY_ARITHM_UNKNOWN: {
                val_del(y);
                break;
            }
        }
    }
    val_del(args);
    return x;
}

Value* builtin_add(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_op(env, args, "+");
}

Value* builtin_sub(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_op(env, args, "-");
}

Value* builtin_mul(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_op(env, args, "*");
}

Value* builtin_div(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_op(env, args, "/");
}

Value* builtin_mod(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_op(env, args, "%");
}

Value* builtin_pow(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_op(env, args, "^");
}

Value* builtin_max(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_op(env, args, "max");
}

Value* builtin_min(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    return builtin_op(env, args, "min");
}

Value* builtin_sum(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);
    Value* list = args->cell[0];
    double dec_sum = 0;
    long int_sum = 0;
    bool dec_flag = false;
    for (int i = 0; i < list->count; i++) {
        LCHECK(
            args,
            list->cell[i]->type == LISPY_VAL_NUM
                || list->cell[i]->type == LISPY_VAL_DEC,
            "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s or %s.",
            __func__,
            i,
            ltype_name(list->cell[i]->type),
            ltype_name(LISPY_VAL_NUM),
            ltype_name(LISPY_VAL_DEC)
        );
        switch (list->cell[i]->type) {
            case LISPY_VAL_NUM: {
                dec_sum += (double)list->cell[i]->num;
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
    val_del(args);
    return dec_flag ? val_dec(dec_sum) : val_num(int_sum);
}

Value* builtin_prod(Environment* env, Value* args) {
    ASSERT(env != NULL);
    ASSERT(args != NULL);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);
    Value* list = args->cell[0];
    double dec_prod = 1;
    long int_prod = 1;
    bool dec_flag = false;
    for (int i = 0; i < list->count; i++) {
        LCHECK(
            args,
            list->cell[i]->type == LISPY_VAL_NUM
                || list->cell[i]->type == LISPY_VAL_DEC,
            "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s or %s.",
            __func__,
            i,
            ltype_name(list->cell[i]->type),
            ltype_name(LISPY_VAL_NUM),
            ltype_name(LISPY_VAL_DEC)
        );
        switch (list->cell[i]->type) {
            case LISPY_VAL_NUM: {
                dec_prod *= (double)list->cell[i]->num;
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
    val_del(args);
    return dec_flag ? val_dec(dec_prod) : val_num(int_prod);
}
