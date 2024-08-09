#pragma once

#include "klangc.h"

klangc_pat_appl_t *klangc_pat_appl_new(klangc_pat_t *constr, klangc_pat_t *arg);

klangc_pat_t *klangc_pat_appl_get_constr(klangc_pat_appl_t *appl);

klangc_pat_t *klangc_pat_appl_get_arg(klangc_pat_appl_t *appl);

klangc_bind_result_t klangc_pat_appl_bind(klangc_expr_env_t *env,
                                          klangc_pat_appl_t *pat_appl,
                                          klangc_expr_ref_target_t *target);