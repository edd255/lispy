#include "common.h"
#include "io.h"

//==== READING =================================================================

Value* val_read_num(const mpc_ast_t* tree) {
    ASSERT(tree != NULL);
    errno = 0;
    if (strchr(tree->contents, '.') == NULL) {
        long x = strtol(tree->contents, NULL, 10);
        return errno != ERANGE ? val_num(x) : val_err("invalid number");
    }
    double x = strtod(tree->contents, NULL);
    return errno != ERANGE ? val_dec(x) : val_err("invalid number");
}

Value* val_read(mpc_ast_t* tree) {
    ASSERT(tree != NULL);

    // If Symbol, String or Number return conversion to that type
    if (strstr(tree->tag, "number")) {
        return val_read_num(tree);
    }
    if (strstr(tree->tag, "string")) {
        return val_read_str(tree);
    }
    if (strstr(tree->tag, "symbol")) {
        return val_sym(tree->contents);
    }
    // If root (>) or sexpr then create empty list
    Value* x = NULL;
    if (strcmp(tree->tag, ">") == 0) {
        x = val_sexpr();
    }
    if (strstr(tree->tag, "sexpr")) {
        x = val_sexpr();
    }
    if (strstr(tree->tag, "qexpr")) {
        x = val_qexpr();
    }
    // Fill this list with any valid expression contained within
    for (int i = 0; i < tree->children_num; i++) {
        if (strcmp(tree->children[i]->contents, "(") == 0) {
            continue;
        }
        if (strcmp(tree->children[i]->contents, ")") == 0) {
            continue;
        }
        if (strcmp(tree->children[i]->contents, "{") == 0) {
            continue;
        }
        if (strcmp(tree->children[i]->contents, "}") == 0) {
            continue;
        }
        if (strcmp(tree->children[i]->tag, "regex") == 0) {
            continue;
        }
        if (strstr(tree->children[i]->tag, "comment")) {
            continue;
        }
        x = val_add(x, val_read(tree->children[i]));
    }
    return x;
}

Value* val_read_str(mpc_ast_t* tree) {
    ASSERT(tree != NULL);
    // Cut off final quote character
    tree->contents[strlen(tree->contents) - 1] = '\0';
    // Copy the string missing out the first quote character
    size_t unescaped_size = strlen(tree->contents + 1) + 1;
    char* unescaped = MALLOC(unescaped_size);
    strlcpy(unescaped, tree->contents + 1, unescaped_size);
    // Pass through the unescape function
    unescaped = mpcf_unescape(unescaped);
    // Construct a new Value using the string
    Value* str = val_str(unescaped);
    // Free the string and return
    FREE(unescaped);
    return str;
}
