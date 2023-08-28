#include "builtins/list.h"

#include "eval.h"
#include "io.h"

//==== List functions ==========================================================
lval_t* builtin_list(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);

    args->type = LISPY_VAL_QEXPR;
    return args;
}

lval_t* builtin_head(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    LCHECK_NOT_EMPTY(__func__, args, 0);

    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            // Take the first argument
            lval_t* v = lval_take(args, 0);

            // Delete all elements that are not head and return
            while (v->count > 1) {
                lval_del(lval_pop(v, 1));
            }
            return v;
        }
        case LISPY_VAL_STR: {
            const char letter[2] = {(args->cell[0]->str)[0], '\0'};
            lval_del(args);
            return lval_str(letter);
        }
    }
    return lval_err(
        "Function %s expected a string or a q-expression",
        __func__
    );
}

lval_t* builtin_tail(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    LCHECK_NOT_EMPTY(__func__, args, 0);

    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            // Take first argument
            lval_t* v = lval_take(args, 0);

            // Delete first element and return
            lval_t* y = lval_pop(v, 0);
            assert(NULL != y);
            lval_del(y);
            return v;
        }
        case LISPY_VAL_STR: {
            char* str = (args->cell[0]->str);
            if (NULL == str || '\0' == str[0] || '\0' == str[1]) {
                lval_del(args);
                return lval_err(
                    "Function '%s' needs a longer string",
                    __func__
                );
            }
            size_t new_length = strlen(str) - 1;
            char* tail_str = MALLOC(new_length + 1);
            strcpy(tail_str, str + 1);
            lval_t* tail = lval_str(tail_str);
            free(tail_str);
            lval_del(args);
            return tail;
        }
    }
    lval_del(args);
    return lval_err(
        "Function '%s' expected a string or a q-expression",
        __func__
    );
}

lval_t* builtin_eval(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);

    lval_t* x = lval_take(args, 0);
    x->type = LISPY_VAL_SEXPR;
    return lval_eval(env, x);
}

lval_t* builtin_join(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            for (int i = 0; i < args->count; i++) {
                LCHECK_TYPE(__func__, args, i, LISPY_VAL_QEXPR);
            }
            break;
        }
        case LISPY_VAL_STR: {
            for (int i = 0; i < args->count; i++) {
                LCHECK_TYPE(__func__, args, i, LISPY_VAL_STR);
            }
            break;
        }
    }
    lval_t* x = lval_pop(args, 0);
    assert(NULL != x);
    while (args->count) {
        x = lval_join(x, lval_pop(args, 0));
    }
    lval_del(args);
    return x;
}

lval_t* builtin_cons(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);
    UNUSED(env);

    if (LISPY_VAL_QEXPR != args->cell[0]->type) {
        lval_t* x = lval_add(lval_qexpr(), lval_pop(args, 0));
        x = lval_join(x, lval_take(args, 0));
        return x;
    } else {
        lval_t* x = lval_pop(args, 0);
        x = lval_join(x, lval_take(args, 0));
        return x;
    }
}

lval_t* builtin_len(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    UNUSED(env);
    lval_t* num = NULL;
    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            num = lval_num(args->cell[0]->count);
            break;
        }
        case LISPY_VAL_STR: {
            assert(NULL != args->cell[0]->str);
            num = lval_num(strlen(args->cell[0]->str));
            break;
        }
    }
    lval_del(args);
    return num;
}

lval_t* builtin_pack(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_FN);
    lval_t* eval = lval_sexpr();
    lval_add(eval, lval_pop(args, 0));
    lval_t* packed = lval_qexpr();
    while (args->count) {
        lval_add(packed, lval_pop(args, 0));
    }
    lval_add(eval, packed);
    lval_del(args);
    return lval_eval_sexpr(env, eval);
}

lval_t* builtin_unpack(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);

    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_FN);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_QEXPR);
    LCHECK_NOT_EMPTY(__func__, args, 1);

    lval_t* eval = lval_sexpr();
    lval_add(eval, lval_pop(args, 0));
    lval_t* x = lval_take(args, 0);

    while (x->count) {
        lval_add(eval, lval_pop(x, 0));
    }
    lval_del(x);
    return lval_eval_sexpr(env, eval);
}

lval_t* builtin_nth(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);

    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_NUM);
    LCHECK_TYPES(__func__, args, 1, LISPY_VAL_QEXPR, LISPY_VAL_STR);

    long idx = args->cell[0]->num;
    lval_t* arg = args->cell[1];
    switch (arg->type) {
        case LISPY_VAL_QEXPR: {
            LCHECK_IDX_QEXPR(__func__, args, 1, idx);
            lval_t* nth_element = lval_pop(arg, 0);
            for (int i = idx; i > 0; i--) {
                lval_del(nth_element);
                nth_element = lval_pop(arg, 0);
            }
            lval_del(args);
            return nth_element;
        }
        case LISPY_VAL_STR: {
            return NULL;
        }
    }
    lval_del(args);
    return lval_err(
        "'%s' expected Q-Expression or String but got %s",
        __func__,
        ltype_name(arg->type)
    );
}
