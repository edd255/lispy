#ifndef LISPY_BUILTINS_H
#define LISPY_BUILTINS_H

#include "builtins/arithmetic.h"
#include "builtins/conditional.h"
#include "builtins/env.h"
#include "builtins/eq_cmp.h"
#include "builtins/helpers.h"
#include "builtins/list.h"
#include "builtins/logic.h"
#include "builtins/magn_cmp.h"
#include "builtins/strings.h"
#include "builtins/vars.h"
#include "common.h"

void lenv_add_builtins(lenv_t* env);
void lenv_add_builtin(lenv_t* env, char* name, lbuiltin_t fn);

#endif
