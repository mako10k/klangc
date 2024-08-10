#pragma once

#include "klangc.h"
#include "types.h"

klangc_pat_alge_t *klangc_pat_alge_new(klangc_symbol_t *constr);

void klangc_pat_alge_add_arg(klangc_pat_alge_t *base, int argc,
                             klangc_pat_t **args);

klangc_symbol_t *klangc_pat_alge_get_constr(klangc_pat_alge_t *alge);

int klangc_pat_alge_get_argc(klangc_pat_alge_t *alge);

klangc_pat_t *klangc_pat_alge_get_arg(klangc_pat_alge_t *alge, int index);

void klangc_pat_alge_print(klangc_output_t *output, int prec,
                           klangc_pat_alge_t *pat_alge);

klangc_parse_result_t klangc_pat_alge_parse(klangc_input_t *input, int noarg,
                                            klangc_pat_alge_t **ppat);

klangc_bind_result_t klangc_pat_alge_bind(klangc_expr_env_t *env,
                                          klangc_pat_alge_t *pat_appl,
                                          klangc_expr_ref_target_t *target);