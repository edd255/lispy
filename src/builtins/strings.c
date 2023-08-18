#include "strings.h"

#include <assert.h>

#include "../eval.h"
#include "../main.h"
#include "../printing.h"
#include "../reading.h"
#include "../values.h"

//==== String functions ========================================================
lval_t* builtin_load(lenv_t* e, lval_t* a) {
    assert(NULL != e);
    assert(NULL != a);
    LASSERT_NUM(__func__, a, 1);
    LASSERT_TYPE(__func__, a, 0, LVAL_STR);

    // Parse file given by string name
    mpc_result_t r;
    mpc_parser_t* lispy = get_lispy_parser();
    if (mpc_parse_contents(a->cell[0]->str, lispy, &r)) {
        // Read contents
        assert(NULL != r.output);
        lval_t* expr = lval_read(r.output);
        assert(expr != NULL);
        mpc_ast_delete(r.output);

        // Evaluate each expression
        while (expr->count) {
            lval_t* x = lval_eval(e, lval_pop(expr, 0));

            // If evaluation leads to error print it
            if (LVAL_ERR == x->type) {
                lval_println(x);
            }
            lval_del(x);
        }
        // Delete expressions and arguments
        lval_del(expr);
        lval_del(a);

        // Return empty list
        return lval_sexpr();
    }  // Get parse error as string
    char* err_msg = mpc_err_string(r.error);
    mpc_err_delete(r.error);

    // Create new error message using it
    lval_t* err = lval_err("Could not load library %s", err_msg);
    LOG_FREE(err_msg);
    lval_del(a);

    // Cleanup and return error
    return err;
}

lval_t* builtin_print(lenv_t* e, lval_t* a) {
    assert(NULL != e);
    assert(NULL != a);
    UNUSED(e);

    // Print each argument followed by a space
    for (int i = 0; i < a->count; i++) {
        lval_print(a->cell[i]);
        putchar(' ');
    }
    // Print a newline and delete arguments
    putchar('\n');
    lval_del(a);
    return lval_sexpr();
}

lval_t* builtin_error(lenv_t* e, lval_t* a) {
    assert(NULL != e);
    assert(NULL != a);
    UNUSED(e);
    LASSERT_NUM(__func__, a, 1);
    LASSERT_TYPE(__func__, a, 0, LVAL_STR);

    // Construct error from first argument
    lval_t* err = lval_err(a->cell[0]->str);

    // Delete arguments and return
    lval_del(a);
    return err;
}
