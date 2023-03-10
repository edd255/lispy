#ifndef CLISP_EVAL_H
#define CLISP_EVAL_H

#include <errno.h>
#include "../deps/mpc/mpc.h"
#include "lisp_value.h"

#define LISP_EVAL_ASSERT(args, cond, err) \
        if (!(cond)) { lisp_value.free(args); return lisp_value.error(err); }

typedef struct {
        /* Evaluate S-expressions */
        lisp_value_t* (* const sexpr)(lisp_value_t* value);

        /* Evaluate expressions */
        lisp_value_t* (* const eval)(lisp_value_t* value);

        /* Apply operator */
        lisp_value_t* (* const op)(lisp_value_t* a, char* op);

	/*
         * The head function repeatedly pops and deletes the item at index 1
         * until there is nothing else left in the list.
         */
	lisp_value_t* (* const builtin_head)(lisp_value_t* a);

        /*
         * The tail function pops and deletes the item at index 0, leaving the 
         * tail remaining. 
         */
        lisp_value_t* (* const builtin_tail)(lisp_value_t* a);

	/*
         * The list function converts the input S-Expression to a Q-Expression
         * and returns it.
         */
	lisp_value_t* (* const builtin_list)(lisp_value_t* a);

        /*
         * The eval function takes as input some single Q-Expression, which it
         * converts to an S-Expression, and evaluates using lval_eval.
         */
        lisp_value_t* (* const builtin_eval)(lisp_value_t* a);

        /*
         * The join function checks that all of the arguments are Q-Expressions
         * and then we join them together one by one.
         */
        lisp_value_t* (* const builtin_join)(lisp_value_t* a);

        /* Call the correct function depending on what symbol we encounter in evaluation */
        lisp_value_t* (* const bultin)(lisp_value_t* a, char* func);
} namespace_lisp_eval;

extern namespace_lisp_eval const lisp_eval;

#endif
