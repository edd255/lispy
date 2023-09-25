#include "common.h"
#include "io.h"

//==== READING =================================================================

lval* lval_read_num(const mpc_ast_t* tree) {
    assert(NULL != tree);
    errno = 0;
    if (NULL == strchr(tree->contents, '.')) {
        long x = strtol(tree->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
    }
    double x = strtod(tree->contents, NULL);
    return errno != ERANGE ? lval_dec(x) : lval_err("invalid number");
}

lval* lval_read(mpc_ast_t* tree) {
    assert(NULL != tree);

    // If Symbol, String or Number return conversion to that type
    if (strstr(tree->tag, "number")) {
        return lval_read_num(tree);
    }
    if (strstr(tree->tag, "string")) {
        return lval_read_str(tree);
    }
    if (strstr(tree->tag, "symbol")) {
        return lval_sym(tree->contents);
    }
    // If root (>) or sexpr then create empty list
    lval* x = NULL;
    if (0 == strcmp(tree->tag, ">")) {
        x = lval_sexpr();
    }
    if (strstr(tree->tag, "sexpr")) {
        x = lval_sexpr();
    }
    if (strstr(tree->tag, "qexpr")) {
        x = lval_qexpr();
    }
    // Fill this list with any valid expression contained within
    for (int i = 0; i < tree->children_num; i++) {
        if (0 == strcmp(tree->children[i]->contents, "(")) {
            continue;
        }
        if (0 == strcmp(tree->children[i]->contents, ")")) {
            continue;
        }
        if (0 == strcmp(tree->children[i]->contents, "{")) {
            continue;
        }
        if (0 == strcmp(tree->children[i]->contents, "}")) {
            continue;
        }
        if (0 == strcmp(tree->children[i]->tag, "regex")) {
            continue;
        }
        if (strstr(tree->children[i]->tag, "comment")) {
            continue;
        }
        x = lval_add(x, lval_read(tree->children[i]));
    }
    return x;
}

lval* lval_read_str(mpc_ast_t* tree) {
    assert(NULL != tree);

    // Cut off final quote character
    tree->contents[strlen(tree->contents) - 1] = '\0';

    // Copy the string missing out the first quote character
    size_t unescaped_size = strlen(tree->contents + 1) + 1;
    char* unescaped = MALLOC(unescaped_size);
    strlcpy(unescaped, tree->contents + 1, unescaped_size);

    // Pass through the unescape function
    unescaped = mpcf_unescape(unescaped);

    // Construct a new lval using the string
    lval* str = lval_str(unescaped);

    // Free the string and return
    FREE(unescaped);
    return str;
}
