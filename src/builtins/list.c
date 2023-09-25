#include "builtins/list.h"

#include "builtins/eq_cmp.h"
#include "eval.h"
#include "io.h"

//==== List functions ==========================================================
lval* builtin_list(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    args->type = LISPY_VAL_QEXPR;
    return args;
}

lval* builtin_head(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            LCHECK_QEXPR_NOT_EMPTY(__func__, args, 0);

            // Take the first argument
            lval* v = lval_take(args, 0);

            // Delete all elements that are not head and return
            while (v->count > 1) {
                lval_del(lval_pop(v, 1));
            }
            return v;
        }
        case LISPY_VAL_STR: {
            LCHECK_STR_NOT_EMPTY(__func__, args, 0);
            const char letter[2] = {(args->cell[0]->str)[0], '\0'};
            lval_del(args);
            return lval_str(letter);
        }
    }
    lval* err = lval_err(
        "Function %s expected a string or a q-expression but got %s",
        __func__,
        ltype_name(args->cell[1]->type)
    );
    lval_del(args);
    return err;
}

lval* builtin_tail(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            LCHECK_QEXPR_NOT_EMPTY(__func__, args, 0);

            // Take first argument
            lval* v = lval_take(args, 0);

            // Delete first element and return
            lval* y = lval_pop(v, 0);
            assert(NULL != y);
            lval_del(y);
            return v;
        }
        case LISPY_VAL_STR: {
            LCHECK_STR_NOT_EMPTY(__func__, args, 0);
            char* str = (args->cell[0]->str);
            if (NULL == str || '\0' == str[0] || '\0' == str[1]) {
                lval_del(args);
                return lval_err(
                    "Function '%s' needs a longer string",
                    __func__
                );
            }
            size_t new_len = args->cell[0]->len;
            char* tail_str = MALLOC(new_len + 1);
            strlcpy(tail_str, str + 1, new_len + 1);
            lval* tail = lval_str(tail_str);
            FREE(tail_str);
            lval_del(args);
            return tail;
        }
    }
    lval* err = lval_err(
        "Function '%s' expected a string or a q-expression",
        __func__,
        ltype_name(args->cell[1]->type)
    );
    lval_del(args);
    return err;
}

lval* builtin_eval(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);
    lval* x = lval_take(args, 0);
    x->type = LISPY_VAL_SEXPR;
    return lval_eval(env, x);
}

lval* builtin_join(lenv* env, lval* args) {
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
    lval* x = lval_pop(args, 0);
    assert(NULL != x);
    while (args->count) {
        x = lval_join(x, lval_pop(args, 0));
    }
    lval_del(args);
    return x;
}

lval* builtin_cons(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);
    UNUSED(env);
    if (LISPY_VAL_QEXPR != args->cell[0]->type) {
        lval* x = lval_add(lval_qexpr(), lval_pop(args, 0));
        x = lval_join(x, lval_take(args, 0));
        return x;
    }
    lval* x = lval_pop(args, 0);
    x = lval_join(x, lval_take(args, 0));
    return x;
}

lval* builtin_len(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    UNUSED(env);
    lval* num = NULL;
    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            num = lval_num(args->cell[0]->count);
            break;
        }
        case LISPY_VAL_STR: {
            assert(NULL != args->cell[0]->str);
            num = lval_num(args->cell[0]->len);
            break;
        }
    }
    lval_del(args);
    return num;
}

lval* builtin_pack(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_FN);
    lval* eval = lval_sexpr();
    lval_add(eval, lval_pop(args, 0));
    lval* packed = lval_qexpr();
    while (args->count) {
        lval_add(packed, lval_pop(args, 0));
    }
    lval_add(eval, packed);
    lval_del(args);
    return lval_eval_sexpr(env, eval);
}

lval* builtin_unpack(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_FN);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_QEXPR);
    LCHECK_QEXPR_NOT_EMPTY(__func__, args, 1);

    lval* eval = lval_sexpr();
    lval_add(eval, lval_pop(args, 0));
    lval* x = lval_take(args, 0);

    while (x->count) {
        lval_add(eval, lval_pop(x, 0));
    }
    lval_del(x);
    return lval_eval_sexpr(env, eval);
}

lval* builtin_nth(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_NUM);
    LCHECK_TYPES(__func__, args, 1, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    long idx = args->cell[0]->num;
    lval* arg = args->cell[1];
    switch (arg->type) {
        case LISPY_VAL_QEXPR: {
            LCHECK_IDX_QEXPR(__func__, args, 1, idx);
            lval* nth_element = lval_pop(arg, 0);
            for (int i = idx; i > 0; i--) {
                lval_del(nth_element);
                nth_element = lval_pop(arg, 0);
            }
            lval_del(args);
            return nth_element;
        }
        case LISPY_VAL_STR: {
            LCHECK_IDX_STR(__func__, args, 1, (unsigned long)idx);
            const char nth_element[2] = {(args->cell[1]->str)[idx], '\0'};
            lval_del(args);
            return lval_str(nth_element);
        }
    }
    lval* err = lval_err(
        "'%s' expected Q-Expression or String but got %s",
        __func__,
        ltype_name(arg->type)
    );
    lval_del(args);
    return err;
}

lval* builtin_first(lenv* env, lval* args) {
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    lval* nth_args = lval_qexpr();
    nth_args = lval_add(nth_args, lval_num(0));
    nth_args = lval_add(nth_args, lval_pop(args, 0));
    lval* fst = builtin_nth(env, nth_args);
    lval_del(args);
    return fst;
}

lval* builtin_second(lenv* env, lval* args) {
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    lval* nth_args = lval_qexpr();
    nth_args = lval_add(nth_args, lval_num(1));
    nth_args = lval_add(nth_args, lval_pop(args, 0));
    lval* snd = builtin_nth(env, nth_args);
    lval_del(args);
    return snd;
}

lval* builtin_third(lenv* env, lval* args) {
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    lval* nth_args = lval_qexpr();
    nth_args = lval_add(nth_args, lval_num(2));
    nth_args = lval_add(nth_args, lval_pop(args, 0));
    lval* trd = builtin_nth(env, nth_args);
    lval_del(args);
    return trd;
}

lval* builtin_last(lenv* env, lval* args) {
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    lval* nth_args = lval_qexpr();
    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            nth_args = lval_add(nth_args, lval_num(args->cell[0]->count - 1));
            nth_args = lval_add(nth_args, lval_pop(args, 0));
            lval* last = builtin_nth(env, nth_args);
            lval_del(args);
            return last;
        }
        case LISPY_VAL_STR: {
            size_t len = args->cell[0]->len;
            nth_args = lval_add(nth_args, lval_num(len - 1));
            nth_args = lval_add(nth_args, lval_pop(args, 0));
            lval* last = builtin_nth(env, nth_args);
            lval_del(args);
            return last;
        }
    }
    lval* err = lval_err(
        "'%s' expected Q-Expression or String but got %s",
        __func__,
        ltype_name(args->cell[0]->type)
    );
    lval_del(args);
    return err;
}

lval* builtin_elem(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_NUM, LISPY_VAL_STR);
    LCHECK_TYPES(__func__, args, 1, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    const lval* needle = args->cell[0];
    lval* haystack = args->cell[1];
    switch (needle->type) {
        case LISPY_VAL_STR: {
            LCHECK_TYPE(__func__, args, 1, LISPY_VAL_STR);
            const char* ptr = strstr(haystack->str, needle->str);
            lval_del(args);
            return lval_num(NULL != ptr);
        }
        case LISPY_VAL_NUM: {
            for (int i = 0; i < haystack->count; i++) {
                lval* cp_elem = lval_copy(needle);
                lval* list_elem = lval_copy(haystack->cell[i]);
                lval* eq_args = lval_qexpr();
                eq_args = lval_add(eq_args, cp_elem);
                eq_args = lval_add(eq_args, list_elem);
                lval* res = builtin_eq(env, eq_args);
                if (res->num == 1) {
                    lval_del(args);
                    return res;
                }
                lval_del(res);
            }
            lval_del(args);
            return lval_num(0);
        }
    }
    lval* err = lval_err(
        "'%s' expected number or string but got %s",
        __func__,
        ltype_name(needle->type)
    );
    lval_del(args);
    return err;
}

lval* builtin_init(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            LCHECK_QEXPR_NOT_EMPTY(__func__, args, 0);
            lval* value = lval_take(args, 0);
            lval_del(lval_pop(value, value->count - 1));
            return value;
        }
        case LISPY_VAL_STR: {
            LCHECK_STR_NOT_EMPTY(__func__, args, 0);
            char* old_str = args->cell[0]->str;
            size_t length = args->cell[0]->len;
            if (length == 1) {
                lval_del(args);
                return lval_str("");
            }
            char* init_str = MALLOC(length);
            strlcpy(init_str, old_str, length - 1);
            init_str[length - 1] = '\0';
            lval_del(args);
            lval* init = lval_str(init_str);
            FREE(init_str);
            return init;
        }
    }
    lval* err = lval_err(
        "'%s' expected q-expression or string but got %s",
        __func__,
        ltype_name(args->cell[0]->type)
    );
    lval_del(args);
    return err;
}

lval* builtin_take(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_NUM);
    LCHECK_TYPES(__func__, args, 1, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    lval* num = args->cell[0];
    lval* expr = args->cell[1];
    switch (expr->type) {
        case LISPY_VAL_QEXPR: {
            if (num->num > expr->count) {
                lval* err = lval_err(
                    "'%s' passed %d but qexpr only has %d elements",
                    __func__,
                    num->num,
                    expr->count
                );
                lval_del(args);
                return err;
            }
            lval* res = lval_qexpr();
            for (int i = 0; i < num->num; i++) {
                lval_add(res, lval_copy(expr->cell[i]));
            }
            lval_del(args);
            return res;
        }
        case LISPY_VAL_STR: {
            size_t len = expr->len;
            if ((unsigned long)num->num > len) {
                lval* err = lval_err(
                    "'%s' passed %d but string only has %d char",
                    __func__,
                    num->num,
                    len
                );
                lval_del(args);
                return err;
            }
            char* taken_str = malloc(sizeof(char) * (num->num + 1));
            strlcpy(taken_str, expr->str, (num->num + 1));
            lval_del(args);
            lval* res = lval_str(taken_str);
            FREE(taken_str);
            return res;
        }
    }
    lval* err = lval_err(
        "'%s' expected string or quoted expression but got %s",
        __func__,
        ltype_name(args->cell[1]->type)
    );
    lval_del(args);
    return err;
}

lval* builtin_drop(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_NUM);
    LCHECK_TYPES(__func__, args, 1, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    lval* num = args->cell[0];
    lval* expr = args->cell[1];
    switch (expr->type) {
        case LISPY_VAL_QEXPR: {
            if (num->num > expr->count) {
                lval* err = lval_err(
                    "'%s' passed %d but qexpr only has %d elements",
                    __func__,
                    num->num,
                    expr->count
                );
                lval_del(args);
                return err;
            }
            for (int i = 0; i < num->num; i++) {
                lval_del(lval_pop(expr, 0));
            }
            lval* res = lval_copy(expr);
            lval_del(args);
            return res;
        }
        case LISPY_VAL_STR: {
            size_t len = expr->len;
            if ((unsigned long)num->num > len) {
                lval* err = lval_err(
                    "'%s' passed %d but string only has %d char",
                    __func__,
                    num->num,
                    len
                );
                lval_del(args);
                return err;
            }
            memmove(expr->str, expr->str + num->num, len - num->num + 1);
            lval* res = lval_copy(expr);
            lval_del(args);
            return res;
        }
    }
    lval* err = lval_err(
        "'%s' expected string or quoted expression but got %s",
        __func__,
        ltype_name(args->cell[1]->type)
    );
    lval_del(args);
    return err;
}

lval* builtin_split(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_NUM);
    LCHECK_TYPES(__func__, args, 1, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    lval* num = args->cell[0];
    lval* expr = args->cell[1];
    switch (expr->type) {
        case LISPY_VAL_QEXPR: {
            if (num->num > expr->count) {
                lval* err = lval_err(
                    "'%s' passed %d but qexpr only has %d elements",
                    __func__,
                    num->num,
                    expr->count
                );
                lval_del(args);
                return err;
            }
            lval* res = lval_qexpr();
            res = lval_add(res, lval_qexpr());
            res = lval_add(res, lval_qexpr());
            for (int i = 0; i < num->num; i++) {
                lval_add(res->cell[0], lval_copy(expr->cell[i]));
            }
            for (int i = num->num; i < expr->count; i++) {
                lval_add(res->cell[1], lval_copy(expr->cell[i]));
            }
            lval_del(args);
            return res;
        }
        case LISPY_VAL_STR: {
            size_t len = expr->len;
            if ((unsigned long)num->num > len) {
                lval* err = lval_err(
                    "'%s' passed %d but string only has %d char",
                    __func__,
                    num->num,
                    len
                );
                lval_del(args);
                return err;
            }
            // Prepare resulting qexpr
            lval* res = lval_qexpr();
            res = lval_add(res, lval_qexpr());
            res = lval_add(res, lval_qexpr());
            char* str0 = MALLOC(sizeof(char) * (num->num + 1));
            char* str1 = MALLOC(sizeof(char) * (len - num->num + 1));

            // Copy the strings and add them to the resulting qexpr
            strlcpy(str0, expr->str, (num->num + 1));
            strlcpy(str1, expr->str + (num->num), (len - num->num + 1));
            (void)lval_add(res->cell[0], lval_str(str0));
            (void)lval_add(res->cell[1], lval_str(str1));

            // Cleanup
            lval_del(args);
            FREE(str0);
            FREE(str1);
            return res;
        }
    }
    lval* err = lval_err(
        "'%s' expected string or quoted expression but got %s",
        __func__,
        ltype_name(args->cell[1]->type)
    );
    lval_del(args);
    return err;
}
