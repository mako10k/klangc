#pragma once

#include "klangc.h"
#include "klangc_input.h"
#include "klangc_types.h"

klangc_expr_lambda_t *klangc_expr_lambda_new(klangc_pattern_t *arg,
                                             klangc_expr_t *body,
                                             klangc_closure_t *upper,
                                             klangc_ipos_t ipos);

void klangc_expr_lambda_print(klangc_output_t *output,
                              klangc_expr_lambda_t *lambda);

klangc_pattern_t *klangc_expr_lambda_get_arg(klangc_expr_lambda_t *lambda);

klangc_expr_t *klangc_expr_lambda_get_body(klangc_expr_lambda_t *lambda);

klangc_closure_t *klangc_expr_lambda_get_upper(klangc_expr_lambda_t *lambda);

klangc_ipos_t klangc_expr_lambda_get_ipos(klangc_expr_lambda_t *lambda);