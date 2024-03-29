#include "builtins/strings.h"

#include "eval.h"
#include "io.h"
#include "main.h"

//==== String functions ========================================================
lval* builtin_load(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_STR);

    // Parse file given by string name
    mpc_result_t parse_result;
    mpc_parser_t* lispy = get_lispy_parser();
    if (mpc_parse_contents(args->cell[0]->str, lispy, &parse_result)) {
        // Read contents
        assert(NULL != parse_result.output);
        lval* expr = lval_read(parse_result.output);
        assert(NULL != expr);
        mpc_ast_delete(parse_result.output);

        // Evaluate each expression
        while (expr->count) {
            lval* x = lval_eval(env, lval_pop(expr, 0));

            // If evaluation leads to error print it
            if (LISPY_VAL_ERR == x->type) {
                lval_println(x);
            }
            lval_del(x);
        }
        // Delete expressions and arguments
        lval_del(expr);
        lval_del(args);

        // Return empty list
        return lval_sexpr();
    }
    // Get parse error as string
    char* err_msg = mpc_err_string(parse_result.error);
    mpc_err_delete(parse_result.error);

    // Create new error message using it
    lval* err = lval_err("Could not load library %s", err_msg);
    FREE(err_msg);
    lval_del(args);

    // Cleanup and return error
    return err;
}

lval* builtin_print(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    for (int i = 0; i < args->count; i++) {
        lval_print(args->cell[i]);
        putchar(' ');
    }
    putchar('\n');
    lval_del(args);
    return lval_sexpr();
}

lval* builtin_error(lenv* env, lval* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_STR);
    lval* err = lval_err(args->cell[0]->str);
    lval_del(args);
    return err;
}
