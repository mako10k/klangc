#pragma once

#include "klangc.h"

klangc_lambda_t *klangc_expr_lambda_new(klangc_pattern_t *arg,
                                             klangc_expr_t *body,
                                             klangc_closure_t *upper,
                                             klangc_ipos_t ipos);

klangc_parse_result_t klangc_expr_lambda_parse(klangc_input_t *input,
                                               klangc_closure_t *upper,
                                               klangc_lambda_t **plambda);

void klangc_expr_lambda_print(klangc_output_t *output,
                              klangc_lambda_t *lambda);

klangc_pattern_t *klangc_expr_lambda_get_arg(klangc_lambda_t *lambda);

klangc_expr_t *klangc_expr_lambda_get_body(klangc_lambda_t *lambda);

klangc_closure_t *klangc_expr_lambda_get_upper(klangc_lambda_t *lambda);

klangc_ipos_t klangc_expr_lambda_get_ipos(klangc_lambda_t *lambda);