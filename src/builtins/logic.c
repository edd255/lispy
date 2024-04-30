#include "builtins/logic.h"

#include "builtins/helpers.h"
#include "io.h"

//==== Logical functions =======================================================
Value* builtin_logic(Environment* env, Value* args, char* op) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
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
            return val_err(
                "Function '%s' expected 'not', 'or' or 'and'",
                __func__
            );
        }
    }
    val_del(args);
    return val_num(result);
}

Value* builtin_and(Environment* env, Value* args) {
    return builtin_logic(env, args, "and");
}

Value* builtin_or(Environment* env, Value* args) {
    return builtin_logic(env, args, "or");
}

Value* builtin_not(Environment* env, Value* args) {
    return builtin_logic(env, args, "not");
}
