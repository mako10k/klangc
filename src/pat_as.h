#pragma once

#include "klangc.h"

klangc_pat_as_t *klangc_pat_as_new(klangc_pat_ref_t *var, klangc_pat_t *pat);

klangc_pat_ref_t *klangc_pat_as_get_ref(klangc_pat_as_t *as);

klangc_pat_t *klangc_pat_as_get_pat(klangc_pat_as_t *as);

void klangc_pat_as_print(klangc_output_t *output, klangc_pat_as_t *pat_as);

klangc_bind_result_t klangc_pat_as_bind(klangc_expr_env_t *env,
                                        klangc_pat_as_t *pat_as,
                                        klangc_expr_ref_target_t *target);