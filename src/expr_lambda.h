#pragma once

#include "klangc.h"
#include "types.h"

klangc_expr_lambda_t *klangc_expr_lambda_new(klangc_pat_t *arg,
                                             klangc_expr_t *body,
                                             klangc_expr_lambda_t *next);

klangc_parse_result_t klangc_expr_lambda_parse(klangc_input_t *input,
                                               klangc_expr_lambda_t **plambda);

void klangc_expr_lambda_print(klangc_output_t *output, int prec,
                              klangc_expr_lambda_t *lambda);

klangc_pat_t *klangc_expr_lambda_get_arg(klangc_expr_lambda_t *lambda);

klangc_expr_t *klangc_expr_lambda_get_body(klangc_expr_lambda_t *lambda);

klangc_expr_lambda_t *klangc_expr_lambda_get_next(klangc_expr_lambda_t *lambda);

klangc_expr_closure_t *
klangc_expr_lambda_get_upper(klangc_expr_lambda_t *lambda);

klangc_bind_result_t klangc_expr_lambda_bind(klangc_expr_env_t *env,
                                             klangc_expr_lambda_t *lambda);
