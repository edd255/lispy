#include "reading.h"
#include "values.h"

//==== READING =================================================================

lval_t* lval_read_num(mpc_ast_t* tree) {
    errno = 0;
    long x = strtol(tree -> contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval_t* lval_read(mpc_ast_t* tree) {
    // If Symbol or Number return conversion to that type
    if (strstr(tree -> tag, "number")) { return lval_read_num(tree); }
    if (strstr(tree -> tag, "symbol")) { return lval_sym(tree -> contents); }

    // If root (>) or sexpr then create empty list
    lval_t* x = NULL;
    if (strcmp(tree -> tag, ">") == 0) { x = lval_sexpr(); }
    if (strstr(tree -> tag, "sexpr"))  { x = lval_sexpr(); }
    if (strstr(tree -> tag, "qexpr"))  { x = lval_qexpr(); }
    if (strstr(tree -> tag, "string")) { return lval_read_str(tree); }

    // Fill this list with any valid expression contained within
    for (int i = 0; i < tree -> children_num; i++) {
        if (strcmp(tree -> children[i] -> contents, "(") == 0)  { continue; }
        if (strcmp(tree -> children[i] -> contents, ")") == 0)  { continue; }
        if (strcmp(tree -> children[i] -> contents, "{") == 0)  { continue; }
        if (strcmp(tree -> children[i] -> contents, "}") == 0)  { continue; }
        if (strcmp(tree -> children[i] -> tag,  "regex") == 0)  { continue; }
        if (strcmp(tree -> children[i] -> tag, "comment") == 0) { continue; }
        x = lval_add(x, lval_read(tree -> children[i]));
    }
    return x;
}

lval_t* lval_read_str(mpc_ast_t* tree) {
    // Cut off final quote character
    tree -> contents[strlen(tree -> contents) - 1] = '\0';
    // Copy the string missing out the first quote character
    char* unescaped = malloc(strlen(tree -> contents + 1) + 1);
    strcpy(unescaped, tree -> contents + 1);
    
    // Pass through the unescape function
    unescaped = mpcf_unescape(unescaped);

    // Construct a new lval using the string
    lval_t* str = lval_str(unescaped);

    // Free the string and return
    free(unescaped);
    return str;
}
