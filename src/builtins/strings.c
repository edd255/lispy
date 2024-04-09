#include "builtins/strings.h"

#include "eval.h"
#include "io.h"
#include "main.h"

//==== String functions ========================================================
Value* builtin_load(Environment* env, Value* args) {
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
        Value* expr = val_read(parse_result.output);
        assert(NULL != expr);
        mpc_ast_delete(parse_result.output);

        // Evaluate each expression
        while (expr->count) {
            Value* x = val_eval(env, val_pop(expr, 0));

            // If evaluation leads to error print it
            if (LISPY_VAL_ERR == x->type) {
                val_println(x);
            }
            val_del(x);
        }
        // Delete expressions and arguments
        val_del(expr);
        val_del(args);

        // Return empty list
        return val_sexpr();
    }
    // Get parse error as string
    char* err_msg = mpc_err_string(parse_result.error);
    mpc_err_delete(parse_result.error);
    // Create new error message using it
    Value* err = val_err("Could not load library %s", err_msg);
    FREE(err_msg);
    val_del(args);
    // Cleanup and return error
    return err;
}

Value* builtin_print(Environment* env, Value* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    for (int i = 0; i < args->count; i++) {
        val_print(args->cell[i]);
        putchar(' ');
    }
    putchar('\n');
    val_del(args);
    return val_sexpr();
}

Value* builtin_error(Environment* env, Value* args) {
    assert(NULL != env);
    assert(NULL != args);
    UNUSED(env);
    LCHECK_NUM(__func__, args, 1);
    LCHECK_TYPE(__func__, args, 0, LISPY_VAL_STR);
    Value* err = val_err(args->cell[0]->str);
    val_del(args);
    return err;
}
