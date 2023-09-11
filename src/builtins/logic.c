#include "builtins/logic.h"

#include "builtins/helpers.h"
#include "io.h"

//==== Logical functions =======================================================
lval* builtin_logic(lenv* env, lval* args, char* op) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    int result = 0;
    switch (logic_op_from_str(op)) {
        case LISPY_LOGIC_AND: {
            LCHECK_NUM(op, args, 2);
            LCHECK_TYPE(op, args, 0, LISPY_VAL_NUM);
            LCHECK_TYPE(op, args, 1, LISPY_VAL_NUM);
            result = !((0 == args->cell[0]->num) || (0 == args->cell[1]->num));
            break;
        }
        case LISPY_LOGIC_OR: {
            LCHECK_NUM(op, args, 2);
            LCHECK_TYPE(op, args, 0, LISPY_VAL_NUM);
            LCHECK_TYPE(op, args, 1, LISPY_VAL_NUM);
            result = !((0 == args->cell[0]->num) && (0 == args->cell[1]->num));
            break;
        }
        case LISPY_LOGIC_NOT: {
            LCHECK_NUM(op, args, 1);
            LCHECK_TYPE(op, args, 0, LISPY_VAL_NUM);
            result = (0 == args->cell[0]->num);
            break;
        }
        case LISPY_LOGIC_UNKNOWN: {
            return lval_err(
                "Function '%s' expected 'not', 'or' or 'and'",
                __func__
            );
        }
    }
    lval_del(args);
    return lval_num(result);
}

lval* builtin_and(lenv* env, lval* args) {
    return builtin_logic(env, args, "and");
}

lval* builtin_or(lenv* env, lval* args) {
    return builtin_logic(env, args, "or");
}

lval* builtin_not(lenv* env, lval* args) {
    return builtin_logic(env, args, "not");
}
