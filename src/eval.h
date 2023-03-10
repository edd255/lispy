#ifndef CLISP_EVAL_H
#define CLISP_EVAL_H

#include <errno.h>
#include "../deps/mpc/mpc.h"
#include "lisp_value.h"

typedef struct {
        /* Evaluate S-expressions */
        lisp_value_t* (* const sexpr)(lisp_value_t* value);

        /* Evaluate expressions */
        lisp_value_t* (* const eval)(lisp_value_t* value);
} namespace_lisp_eval;

extern namespace_lisp_eval const lisp_eval;

#endif
