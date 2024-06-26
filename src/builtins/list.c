#include "builtins/list.h"

#include "builtins/eq_cmp.h"
#include "common.h"
#include "eval.h"
#include "io.h"

//==== List functions ==========================================================
Value* builtin_list(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    args->type = LISPY_VAL_QEXPR;
    return args;
}

Value* builtin_head(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            LCHECK_QEXPR_NOT_EMPTY(__func__, args, 0);
            // Take the first argument
            Value* v = val_take(args, 0);
            // Delete all elements that are not head and return
            while (v->count > 1) {
                val_del(val_pop(v, 1));
            }
            return v;
        }
        case LISPY_VAL_STR: {
            LCHECK_STR_NOT_EMPTY(__func__, args, 0);
            const char letter[2] = {(args->cell[0]->str)[0], '\0'};
            val_del(args);
            return val_str(letter);
        }
    }
    Value* err = val_err(
        "Function %s expected a string or a q-expression but got %s",
        __func__,
        ltype_name(args->cell[1]->type)
    );
    val_del(args);
    return err;
}

Value* builtin_tail(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            LCHECK_QEXPR_NOT_EMPTY(__func__, args, 0);
            // Take first argument
            Value* v = val_take(args, 0);
            // Delete first element and return
            Value* y = val_pop(v, 0);
            ASSERT(NULL != y);
            val_del(y);
            return v;
        }
        case LISPY_VAL_STR: {
            LCHECK_STR_NOT_EMPTY(__func__, args, 0);
            char* str = (args->cell[0]->str);
            if (NULL == str || '\0' == str[0] || '\0' == str[1]) {
                val_del(args);
                return val_err("Function '%s' needs a longer string", __func__);
            }
            size_t new_len = args->cell[0]->len;
            char* tail_str = MALLOC(new_len + 1);
            strlcpy(tail_str, str + 1, new_len + 1);
            Value* tail = val_str(tail_str);
            FREE(tail_str);
            val_del(args);
            return tail;
        }
    }
    Value* err = val_err(
        "Function '%s' expected a string or a q-expression",
        __func__,
        ltype_name(args->cell[1]->type)
    );
    val_del(args);
    return err;
}

Value* builtin_eval(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);
    Value* x = val_take(args, 0);
    x->type = LISPY_VAL_SEXPR;
    return val_eval(env, x);
}

Value* builtin_join(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
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
    Value* x = val_pop(args, 0);
    ASSERT(NULL != x);
    while (args->count) {
        x = val_join(x, val_pop(args, 0));
    }
    val_del(args);
    return x;
}

Value* builtin_cons(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);
    UNUSED(env);
    if (LISPY_VAL_QEXPR != args->cell[0]->type) {
        Value* x = val_add(val_qexpr(), val_pop(args, 0));
        x = val_join(x, val_take(args, 0));
        return x;
    }
    Value* x = val_pop(args, 0);
    x = val_join(x, val_take(args, 0));
    return x;
}

Value* builtin_len(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    UNUSED(env);
    Value* num = NULL;
    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            num = val_num(args->cell[0]->count);
            break;
        }
        case LISPY_VAL_STR: {
            ASSERT(NULL != args->cell[0]->str);
            num = val_num(args->cell[0]->len);
            break;
        }
    }
    val_del(args);
    return num;
}

Value* builtin_pack(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_FN);
    Value* eval = val_sexpr();
    val_add(eval, val_pop(args, 0));
    Value* packed = val_qexpr();
    while (args->count) {
        val_add(packed, val_pop(args, 0));
    }
    val_add(eval, packed);
    val_del(args);
    return val_eval_sexpr(env, eval);
}

Value* builtin_unpack(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_FN);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_QEXPR);
    LCHECK_QEXPR_NOT_EMPTY(__func__, args, 1);
    Value* eval = val_sexpr();
    val_add(eval, val_pop(args, 0));
    Value* x = val_take(args, 0);
    while (x->count) {
        val_add(eval, val_pop(x, 0));
    }
    val_del(x);
    return val_eval_sexpr(env, eval);
}

Value* builtin_nth(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_NUM);
    LCHECK_TYPES(__func__, args, 1, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    long idx = args->cell[0]->num;
    Value* arg = args->cell[1];
    switch (arg->type) {
        case LISPY_VAL_QEXPR: {
            LCHECK_IDX_QEXPR(__func__, args, 1, idx);
            Value* nth_element = val_pop(arg, 0);
            for (int i = idx; i > 0; i--) {
                val_del(nth_element);
                nth_element = val_pop(arg, 0);
            }
            val_del(args);
            return nth_element;
        }
        case LISPY_VAL_STR: {
            LCHECK_IDX_STR(__func__, args, 1, (unsigned long)idx);
            const char nth_element[2] = {(args->cell[1]->str)[idx], '\0'};
            val_del(args);
            return val_str(nth_element);
        }
    }
    Value* err = val_err(
        "'%s' expected Q-Expression or String but got %s",
        __func__,
        ltype_name(arg->type)
    );
    val_del(args);
    return err;
}

Value* builtin_first(Environment* env, Value* args) {
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    Value* nth_args = val_qexpr();
    nth_args = val_add(nth_args, val_num(0));
    nth_args = val_add(nth_args, val_pop(args, 0));
    Value* fst = builtin_nth(env, nth_args);
    val_del(args);
    return fst;
}

Value* builtin_second(Environment* env, Value* args) {
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    Value* nth_args = val_qexpr();
    nth_args = val_add(nth_args, val_num(1));
    nth_args = val_add(nth_args, val_pop(args, 0));
    Value* snd = builtin_nth(env, nth_args);
    val_del(args);
    return snd;
}

Value* builtin_third(Environment* env, Value* args) {
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    Value* nth_args = val_qexpr();
    nth_args = val_add(nth_args, val_num(2));
    nth_args = val_add(nth_args, val_pop(args, 0));
    Value* trd = builtin_nth(env, nth_args);
    val_del(args);
    return trd;
}

Value* builtin_last(Environment* env, Value* args) {
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    Value* nth_args = val_qexpr();
    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            nth_args = val_add(nth_args, val_num(args->cell[0]->count - 1));
            nth_args = val_add(nth_args, val_pop(args, 0));
            Value* last = builtin_nth(env, nth_args);
            val_del(args);
            return last;
        }
        case LISPY_VAL_STR: {
            size_t len = args->cell[0]->len;
            nth_args = val_add(nth_args, val_num(len - 1));
            nth_args = val_add(nth_args, val_pop(args, 0));
            Value* last = builtin_nth(env, nth_args);
            val_del(args);
            return last;
        }
    }
    Value* err = val_err(
        "'%s' expected Q-Expression or String but got %s",
        __func__,
        ltype_name(args->cell[0]->type)
    );
    val_del(args);
    return err;
}

Value* builtin_elem(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_NUM, LISPY_VAL_STR);
    LCHECK_TYPES(__func__, args, 1, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    const Value* needle = args->cell[0];
    Value* haystack = args->cell[1];
    switch (needle->type) {
        case LISPY_VAL_STR: {
            LCHECK_TYPE(__func__, args, 1, LISPY_VAL_STR);
            const char* ptr = strstr(haystack->str, needle->str);
            val_del(args);
            return val_num(NULL != ptr);
        }
        case LISPY_VAL_NUM: {
            for (int i = 0; i < haystack->count; i++) {
                Value* cp_elem = val_copy(needle);
                Value* list_elem = val_copy(haystack->cell[i]);
                Value* eq_args = val_qexpr();
                eq_args = val_add(eq_args, cp_elem);
                eq_args = val_add(eq_args, list_elem);
                Value* res = builtin_eq(env, eq_args);
                if (1 == res->num) {
                    val_del(args);
                    return res;
                }
                val_del(res);
            }
            val_del(args);
            return val_num(0);
        }
    }
    Value* err = val_err(
        "'%s' expected number or string but got %s",
        __func__,
        ltype_name(needle->type)
    );
    val_del(args);
    return err;
}

Value* builtin_init(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            LCHECK_QEXPR_NOT_EMPTY(__func__, args, 0);
            Value* value = val_take(args, 0);
            val_del(val_pop(value, value->count - 1));
            return value;
        }
        case LISPY_VAL_STR: {
            LCHECK_STR_NOT_EMPTY(__func__, args, 0);
            char* old_str = args->cell[0]->str;
            size_t length = args->cell[0]->len;
            if (1 == length) {
                val_del(args);
                return val_str("");
            }
            char* init_str = MALLOC(length);
            strlcpy(init_str, old_str, length - 1);
            init_str[length - 1] = '\0';
            val_del(args);
            Value* init = val_str(init_str);
            FREE(init_str);
            return init;
        }
    }
    Value* err = val_err(
        "'%s' expected q-expression or string but got %s",
        __func__,
        ltype_name(args->cell[0]->type)
    );
    val_del(args);
    return err;
}

Value* builtin_take(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_NUM);
    LCHECK_TYPES(__func__, args, 1, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    Value* num = args->cell[0];
    Value* expr = args->cell[1];
    switch (expr->type) {
        case LISPY_VAL_QEXPR: {
            if (num->num > expr->count) {
                Value* err = val_err(
                    "'%s' passed %d but qexpr only has %d elements",
                    __func__,
                    num->num,
                    expr->count
                );
                val_del(args);
                return err;
            }
            Value* res = val_qexpr();
            for (int i = 0; i < num->num; i++) {
                val_add(res, val_copy(expr->cell[i]));
            }
            val_del(args);
            return res;
        }
        case LISPY_VAL_STR: {
            size_t len = expr->len;
            if ((unsigned long)num->num > len) {
                Value* err = val_err(
                    "'%s' passed %d but string only has %d char",
                    __func__,
                    num->num,
                    len
                );
                val_del(args);
                return err;
            }
            char* taken_str = malloc(sizeof(char) * (num->num + 1));
            strlcpy(taken_str, expr->str, (num->num + 1));
            val_del(args);
            Value* res = val_str(taken_str);
            FREE(taken_str);
            return res;
        }
    }
    Value* err = val_err(
        "'%s' expected string or quoted expression but got %s",
        __func__,
        ltype_name(args->cell[1]->type)
    );
    val_del(args);
    return err;
}

Value* builtin_drop(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_NUM);
    LCHECK_TYPES(__func__, args, 1, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    Value* num = args->cell[0];
    Value* expr = args->cell[1];
    switch (expr->type) {
        case LISPY_VAL_QEXPR: {
            if (num->num > expr->count) {
                Value* err = val_err(
                    "'%s' passed %d but qexpr only has %d elements",
                    __func__,
                    num->num,
                    expr->count
                );
                val_del(args);
                return err;
            }
            for (int i = 0; i < num->num; i++) {
                val_del(val_pop(expr, 0));
            }
            Value* res = val_copy(expr);
            val_del(args);
            return res;
        }
        case LISPY_VAL_STR: {
            size_t len = expr->len;
            if ((unsigned long)num->num > len) {
                Value* err = val_err(
                    "'%s' passed %d but string only has %d char",
                    __func__,
                    num->num,
                    len
                );
                val_del(args);
                return err;
            }
            memmove(expr->str, expr->str + num->num, len - num->num + 1);
            Value* res = val_copy(expr);
            val_del(args);
            return res;
        }
    }
    Value* err = val_err(
        "'%s' expected string or quoted expression but got %s",
        __func__,
        ltype_name(args->cell[1]->type)
    );
    val_del(args);
    return err;
}

Value* builtin_split(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_NUM);
    LCHECK_TYPES(__func__, args, 1, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    Value* num = args->cell[0];
    Value* expr = args->cell[1];
    switch (expr->type) {
        case LISPY_VAL_QEXPR: {
            if (num->num > expr->count) {
                Value* err = val_err(
                    "'%s' passed %d but qexpr only has %d elements",
                    __func__,
                    num->num,
                    expr->count
                );
                val_del(args);
                return err;
            }
            Value* res = val_qexpr();
            res = val_add(res, val_qexpr());
            res = val_add(res, val_qexpr());
            for (int i = 0; i < num->num; i++) {
                val_add(res->cell[0], val_copy(expr->cell[i]));
            }
            for (int i = num->num; i < expr->count; i++) {
                val_add(res->cell[1], val_copy(expr->cell[i]));
            }
            val_del(args);
            return res;
        }
        case LISPY_VAL_STR: {
            size_t len = expr->len;
            if ((unsigned long)num->num > len) {
                Value* err = val_err(
                    "'%s' passed %d but string only has %d char",
                    __func__,
                    num->num,
                    len
                );
                val_del(args);
                return err;
            }
            // Prepare resulting qexpr
            Value* res = val_qexpr();
            res = val_add(res, val_qexpr());
            res = val_add(res, val_qexpr());
            char* str0 = MALLOC(sizeof(char) * (num->num + 1));
            char* str1 = MALLOC(sizeof(char) * (len - num->num + 1));
            // Copy the strings and add them to the resulting qexpr
            strlcpy(str0, expr->str, (num->num + 1));
            strlcpy(str1, expr->str + (num->num), (len - num->num + 1));
            (void)val_add(res->cell[0], val_str(str0));
            (void)val_add(res->cell[1], val_str(str1));
            // Cleanup
            val_del(args);
            FREE(str0);
            FREE(str1);
            return res;
        }
    }
    Value* err = val_err(
        "'%s' expected string or quoted expression but got %s",
        __func__,
        ltype_name(args->cell[1]->type)
    );
    val_del(args);
    return err;
}

Value* builtin_filter(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_FN);
    LCHECK_TYPES(__func__, args, 1, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    Value* fn = args->cell[0];
    switch (args->cell[1]->type) {
        case LISPY_VAL_QEXPR: {
            Value* qexpr = args->cell[1];
            Value* copy = val_copy(args->cell[1]);
            Value* result = val_qexpr();
            for (int i = 0; i < qexpr->count; i++) {
                Value* val = val_eval(env, val_pop(copy, 0));
                Value* arg = val_add(val_sexpr(), val_copy(val));
                Value* lambda = val_copy(fn);
                Value* condition = val_call(env, lambda, arg);
                if (1 == condition->num) {
                    val_add(result, val_copy(val));
                }
                val_del(val);
                val_del(condition);
                val_del(lambda);
            }
            val_del(args);
            val_del(copy);
            return result;
        }
        case LISPY_VAL_STR: {
            const char* str = args->cell[1]->str;
            Value* result = val_qexpr();
            for (unsigned long i = 0; i < strlen(str); i++) {
                Value* lambda = val_copy(fn);
                Value* val = val_str(&str[i]);
                Value* condition = val_call(env, lambda, val);
                if (1 == condition->num) {
                    val_add(result, val_copy(val));
                }
                val_del(val);
                val_del(condition);
                val_del(lambda);
            }
            val_del(args);
            return result;
        }
    }
    Value* err = val_err(
        "'%s' expected string or quoted expression but got %s",
        __func__,
        ltype_name(args->cell[1]->type)
    );
    val_del(args);
    return err;
}

Value* builtin_reverse(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPES(__func__, args, 0, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    switch (args->cell[0]->type) {
        case LISPY_VAL_QEXPR: {
            Value* result = val_qexpr();
            Value* qexpr = args->cell[0];
            for (int i = qexpr->count - 1; i >= 0; i--) {
                val_add(result, val_copy(qexpr->cell[i]));
            }
            val_del(args);
            return result;
        }
        case LISPY_VAL_STR: {
            Value* val = args->cell[0];
            int len = strlen(val->str);
            for (int i = 0, j = len - 1; i <= j; i++, j--) {
                char c = val->str[i];
                val->str[i] = val->str[j];
                val->str[j] = c;
            }
            return val;
        }
    }
    Value* err = val_err(
        "'%s' expected string or quoted expression but got %s",
        __func__,
        ltype_name(args->cell[1]->type)
    );
    val_del(args);
    return err;
}

Value* builtin_map(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_FN);
    LCHECK_TYPES(__func__, args, 1, LISPY_VAL_QEXPR, LISPY_VAL_STR);
    Value* fn = args->cell[0];
    switch (args->cell[1]->type) {
        case LISPY_VAL_QEXPR: {
            Value* qexpr = args->cell[1];
            Value* copy = val_copy(args->cell[1]);
            Value* result = val_qexpr();
            for (int i = 0; i < qexpr->count; i++) {
                Value* val = val_eval(env, val_pop(copy, 0));
                Value* arg = val_add(val_sexpr(), val_copy(val));
                Value* lambda = val_copy(fn);
                Value* res = val_call(env, lambda, arg);
                val_add(result, res);
                val_del(val);
                val_del(lambda);
            }
            val_del(args);
            val_del(copy);
            return result;
        }
        case LISPY_VAL_STR: {
            const char* str = args->cell[1]->str;
            Value* result = val_qexpr();
            for (unsigned long i = 0; i < strlen(str); i++) {
                Value* lambda = val_copy(fn);
                Value* val = val_str(&str[i]);
                Value* res = val_call(env, lambda, val);
                val_add(result, res);
                val_del(val);
                val_del(lambda);
            }
            val_del(args);
            return result;
        }
    }
    Value* err = val_err(
        "'%s' expected string or quoted expression but got %s",
        __func__,
        ltype_name(args->cell[1]->type)
    );
    val_del(args);
    return err;
}

Value* builtin_lookup(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_QEXPR);
    Value* elem = val_eval(env, val_copy(args->cell[0]));
    Value* list = args->cell[1];
    Value* result = NULL;
    for (int i = 0; i < list->count; i++) {
        Value* pair = list->cell[0];
        LCHECK(
            args,
            LISPY_VAL_QEXPR == pair->type,
            "'%s' expected a qexpr at index %d but got %s",
            __func__,
            i,
            ltype_name(pair->type)
        );
        LCHECK(
            args,
            2 == pair->count,
            "'%s' expected a 2 elements at index %d but got %d",
            __func__,
            i,
            pair->count
        );
        Value* key = val_eval(env, val_copy(pair->cell[0]));
        Value* value = pair->cell[1];
        if (val_eq(key, elem)) {
            result = val_copy(value);
            break;
        }
    }
    val_del(args);
    return NULL == result ? val_sexpr() : val_eval(env, result);
}

Value* builtin_zip(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 2);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);
    LCHECK_TYPE(__func__, args, 1, LISPY_VAL_QEXPR);
    Value* result = val_qexpr();
    Value* list1 = args->cell[0];
    Value* list2 = args->cell[1];
    int limit = list1->count < list2->count ? list1->count : list2->count;
    for (int i = 0; i < limit; i++) {
        Value* pair = val_qexpr();
        val_add(pair, val_copy(list1->cell[i]));
        val_add(pair, val_copy(list2->cell[i]));
        val_add(result, pair);
    }
    val_del(args);
    return result;
}

Value* builtin_unzip(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_QEXPR);
    Value* result = val_qexpr();
    Value* list = args->cell[0];
    Value* list1 = val_qexpr();
    Value* list2 = val_qexpr();
    for (int i = 0; i < list->count; i++) {
        Value* pair = list->cell[i];
        LCHECK(
            args,
            2 == pair->count,
            "'%s' expected a pair at index %d but got %d",
            __func__,
            i,
            args->cell[i]->count
        );
        val_add(list1, val_copy(pair->cell[0]));
        val_add(list2, val_copy(pair->cell[1]));
    }
    val_add(result, list1);
    val_add(result, list2);
    val_del(args);
    return result;
}

Value* builtin_foldl(Environment* env, Value* args) {
    ASSERT(NULL != env);
    ASSERT(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 3);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_FN);
    LCHECK_TYPE(__func__, args, 2, LISPY_VAL_QEXPR);
    Value* f = args->cell[0];
    Value* x = val_eval(env, args->cell[1]);
    Value* ys = args->cell[2];
    Value* f_args = val_qexpr();
    val_add(f_args, val_copy(x));
    val_add(f_args, val_copy(ys->cell[0]));
    Value* z = val_call(env, f, f_args);
    for (int i = 1; i < ys->count; i++) {
        f_args = val_qexpr();
        val_add(f_args, z);
        val_add(f_args, val_copy(ys->cell[i]));
        z = val_call(env, f, f_args);
    }
    val_del(args);
    return z;
}
