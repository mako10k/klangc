#pragma once

#include "klangc.h"
#include "types.h"

typedef klangc_value_t *(*klangc_eval_func_t)(klangc_value_t *val);

typedef int (*klangc_match_func_t)(klangc_env_t *env, klangc_pat_t *pat,
                                   klangc_value_t *val);

typedef struct klangc_expr_cfunc_def {
  const char *kecd_name;
  klangc_eval_func_t kecd_eval;
  klangc_match_func_t kecd_match;
} klangc_expr_cfunc_def_t;

klangc_expr_cfunc_t *klangc_expr_cfunc_new(klangc_expr_cfunc_def_t *def);