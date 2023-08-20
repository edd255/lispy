#include "builtins/strings.h"

#include "eval.h"
#include "main.h"
#include "printing.h"
#include "reading.h"
#include "values.h"

//==== String functions ========================================================
lval_t* builtin_load(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    LASSERT_NUM(__func__, args, 1);
    LASSERT_TYPE(__func__, args, 0, LVAL_STR);

    // Parse file given by string name
    mpc_result_t r;
    mpc_parser_t* lispy = get_lispy_parser();
    if (mpc_parse_contents(args->cell[0]->str, lispy, &r)) {
        // Read contents
        assert(NULL != r.output);
        lval_t* expr = lval_read(r.output);
        assert(expr != NULL);
        mpc_ast_delete(r.output);

        // Evaluate each expression
        while (expr->count) {
            lval_t* x = lval_eval(env, lval_pop(expr, 0));

            // If evaluation leads to error print it
            if (LVAL_ERR == x->type) {
                lval_println(x);
            }
            lval_del(x);
        }
        // Delete expressions and arguments
        lval_del(expr);
        lval_del(args);

        // Return empty list
        return lval_sexpr();
    }  // Get parse error as string
    char* err_msg = mpc_err_string(r.error);
    mpc_err_delete(r.error);

    // Create new error message using it
    lval_t* err = lval_err("Could not load library %s", err_msg);
    FREE(err_msg);
    lval_del(args);

    // Cleanup and return error
    return err;
}

lval_t* builtin_print(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);

    // Print each argument followed by a space
    for (int i = 0; i < args->count; i++) {
        lval_print(args->cell[i]);
        putchar(' ');
    }
    // Print a newline and delete arguments
    putchar('\n');
    lval_del(args);
    return lval_sexpr();
}

lval_t* builtin_error(lenv_t* env, lval_t* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LASSERT_NUM(__func__, args, 1);
    LASSERT_TYPE(__func__, args, 0, LVAL_STR);

    // Construct error from first argument
    lval_t* err = lval_err(args->cell[0]->str);

    // Delete arguments and return
    lval_del(args);
    return err;
}
