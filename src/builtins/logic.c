#include "builtins/logic.h"

#include "builtins/helpers.h"
#include "printing.h"

//==== Logical functions =======================================================
lval_t* builtin_logic(lenv_t* env, lval_t* args, char* op) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    int result = 0;
    switch (llogic_from_string(op)) {
        case LLOGIC_AND: {
            LCHECK_NUM(op, args, 2);
            LCHECK_TYPE(op, args, 0, LVAL_NUM);
            LCHECK_TYPE(op, args, 1, LVAL_NUM);
            if ((0 == args->cell[0]->num) || (0 == args->cell[1]->num)) {
                result = 0;
            } else {
                result = 1;
            }
            break;
        }
        case LLOGIC_OR: {
            LCHECK_NUM(op, args, 2);
            LCHECK_TYPE(op, args, 0, LVAL_NUM);
            LCHECK_TYPE(op, args, 1, LVAL_NUM);
            if ((0 == args->cell[0]->num) && (0 == args->cell[1]->num)) {
                result = 0;
            } else {
                result = 1;
            }
            break;
        }
        case LLOGIC_NOT: {
            LCHECK_NUM(op, args, 1);
            LCHECK_TYPE(op, args, 0, LVAL_NUM);
            if (0 == args->cell[0]->num) {
                result = 1;
            } else {
                result = 0;
            }
            break;
        }
        case LLOGIC_UNKNOWN: {
            return lval_err(
                "Function '%s' expected 'not', 'or' or 'and'",
                __func__
            );
        }
    }
    lval_del(args);
    return lval_num(result);
}

lval_t* builtin_and(lenv_t* env, lval_t* args) {
    return builtin_logic(env, args, "and");
}

lval_t* builtin_or(lenv_t* env, lval_t* args) {
    return builtin_logic(env, args, "or");
}

lval_t* builtin_not(lenv_t* env, lval_t* args) {
    return builtin_logic(env, args, "not");
}
