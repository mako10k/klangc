#pragma once

#include "klangc.h"

klangc_expr_env_t *klangc_expr_env_new(klangc_expr_env_t *upper);

klangc_expr_env_t *klangc_expr_env_get_upper(klangc_expr_env_t *env);

klangc_expr_ref_target_t *klangc_expr_env_get_entry(klangc_expr_env_t *env,
                                                    klangc_ref_t *ref);

int klangc_expr_env_put_entry(klangc_expr_env_t *env, klangc_ref_t *ref,
                              klangc_expr_ref_target_t *target);