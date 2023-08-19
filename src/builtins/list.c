#include "list.h"

#include "../eval.h"
#include "../printing.h"
#include "../values.h"

//==== List functions ==========================================================
lval_t* builtin_list(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);
    UNUSED(env);

    a->type = LVAL_QEXPR;
    return a;
}

lval_t* builtin_head(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);
    UNUSED(env);
    LASSERT_NUM(__func__, a, 1);
    LASSERT_TYPES(__func__, a, 0, LVAL_QEXPR, LVAL_STR);
    LASSERT_NOT_EMPTY(__func__, a, 0);

    switch (a->cell[0]->type) {
        case LVAL_QEXPR: {
            // Take the first argument
            lval_t* v = lval_take(a, 0);

            // Delete all elements that are not head and return
            while (v->count > 1) {
                lval_del(lval_pop(v, 1));
            }
            return v;
        }
        case LVAL_STR: {
            char letter[2] = {(a->cell[0]->str)[0], '\0'};
            lval_del(a);
            return lval_str(letter);
        }
    }
    return lval_err(
        "Function %s expected a string or a q-expression",
        __func__
    );
}

lval_t* builtin_tail(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);
    UNUSED(env);
    LASSERT_NUM(__func__, a, 1);
    LASSERT_TYPES(__func__, a, 0, LVAL_QEXPR, LVAL_STR);
    LASSERT_NOT_EMPTY(__func__, a, 0);

    switch (a->cell[0]->type) {
        case LVAL_QEXPR: {
            // Take first argument
            lval_t* v = lval_take(a, 0);

            // Delete first element and return
            lval_t* y = lval_pop(v, 0);
            assert(y != NULL);
            lval_del(y);
            return v;
        }
        case LVAL_STR: {
            char* str = (a->cell[0]->str);
            if (NULL == str || '\0' == str[0] || '\0' == str[1]) {
                lval_del(a);
                return lval_err(
                    "Function '%s' needs a longer string",
                    __func__
                );
            }
            size_t new_length = strlen(str) - 1;
            char* tail_str = malloc(new_length + 1);
            strcpy(tail_str, str + 1);
            lval_t* tail = lval_str(tail_str);
            free(tail_str);
            lval_del(a);
            return tail;
        }
    }
    lval_del(a);
    return lval_err(
        "Function '%s' expected a string or a q-expression",
        __func__
    );
}

lval_t* builtin_eval(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);
    UNUSED(env);
    LASSERT_NUM(__func__, a, 1);
    LASSERT_TYPE(__func__, a, 0, LVAL_QEXPR);

    lval_t* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(env, x);
}

lval_t* builtin_join(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);
    UNUSED(env);

    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPES(__func__, a, i, LVAL_QEXPR, LVAL_STR);
    }
    lval_t* x = lval_pop(a, 0);
    assert(NULL != x);
    while (a->count) {
        x = lval_join(x, lval_pop(a, 0));
    }
    lval_del(a);
    return x;
}

lval_t* builtin_cons(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);
    LASSERT_NUM("cons", a, 2);
    LASSERT_TYPE("cons", a, 0, LVAL_QEXPR);
    UNUSED(env);

    lval_t* x = lval_qexpr();
    if (LVAL_QEXPR != a->cell[0]->type) {
        x = lval_add(x, lval_pop(a, 0));
    } else {
        x = lval_pop(a, 0);
    }
    x = lval_join(x, lval_take(a, 0));
    return x;
}

lval_t* builtin_len(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);
    LASSERT_NUM(__func__, a, 1);
    LASSERT_TYPE(__func__, a, 0, LVAL_QEXPR);
    UNUSED(env);
    lval_t* num = lval_num(a->cell[0]->count);
    lval_del(a);
    return num;
}

lval_t* builtin_pack(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);

    LASSERT_TYPE(__func__, a, 0, LVAL_FN);
    lval_t* eval = lval_sexpr();
    lval_add(eval, lval_pop(a, 0));
    lval_t* packed = lval_qexpr();
    while (a->count) {
        lval_add(packed, lval_pop(a, 0));
    }
    lval_add(eval, packed);
    lval_del(a);
    return lval_eval_sexpr(env, eval);
}

lval_t* builtin_unpack(lenv_t* env, lval_t* a) {
    assert(NULL != env);
    assert(NULL != a);

    LASSERT_NUM(__func__, a, 2);
    LASSERT_TYPE(__func__, a, 0, LVAL_FN);
    LASSERT_TYPE(__func__, a, 1, LVAL_QEXPR);
    LASSERT_NOT_EMPTY(__func__, a, 1);

    lval_t* eval = lval_sexpr();
    lval_add(eval, lval_pop(a, 0));
    lval_t* x = lval_take(a, 0);

    while (x->count) {
        lval_add(eval, lval_pop(x, 0));
    }
    lval_del(x);
    return lval_eval_sexpr(env, eval);
}
