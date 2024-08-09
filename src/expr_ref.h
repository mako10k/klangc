#pragma once

#include "klangc.h"
#include "types.h"

klangc_expr_ref_t *klangc_expr_ref_new(klangc_ref_t *ref);

klangc_ref_t *klangc_expr_ref_get_ref(klangc_expr_ref_t *expr);

klangc_ipos_t klangc_expr_ref_get_ipos(klangc_expr_ref_t *expr);

klangc_expr_ref_target_t *klangc_expr_ref_get_target(klangc_expr_ref_t *expr);

void klangc_expr_ref_set_target(klangc_expr_ref_t *expr,
                                klangc_expr_ref_target_t *target);

klangc_parse_result_t klangc_expr_ref_parse(klangc_input_t *input,
                                            klangc_expr_ref_t **pexpr_ref);

void klangc_expr_ref_print(klangc_output_t *output,
                           klangc_expr_ref_t *expr_ref);

klangc_bind_result_t klangc_expr_ref_bind(klangc_expr_env_t *env,
                                          klangc_expr_ref_t *expr);

klangc_unbound_result_t klangc_expr_ref_check_unbound(klangc_expr_ref_t *expr);