#ifndef HOME_EDD_CODE_LISPY_SRC_BUILTINS_LIST_H
#define HOME_EDD_CODE_LISPY_SRC_BUILTINS_LIST_H

#include "../common.h"

//---- List functions ----------------------------------------------------------
lval_t* builtin_list(lenv_t* e, lval_t* a);
lval_t* builtin_head(lenv_t* e, lval_t* a);
lval_t* builtin_tail(lenv_t* e, lval_t* a);
lval_t* builtin_eval(lenv_t* e, lval_t* a);
lval_t* builtin_join(lenv_t* e, lval_t* a);
lval_t* builtin_cons(lenv_t* e, lval_t* a);
lval_t* builtin_len(lenv_t* e, lval_t* a);
lval_t* builtin_pack(lenv_t* e, lval_t* a);
lval_t* builtin_unpack(lenv_t* e, lval_t* a);

#endif
