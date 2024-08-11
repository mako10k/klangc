#pragma once

#include "expr.h"
#include "klangc.h"
#include "types.h"

klangc_expr_appl_t *klangc_expr_appl_new(klangc_expr_t *func);

void klangc_expr_appl_add_arg(klangc_expr_appl_t *appl, klangc_expr_t *arg);

klangc_expr_t *klangc_expr_appl_get_func(klangc_expr_appl_t *appl);

klangc_expr_t *klangc_expr_appl_get_arg(klangc_expr_appl_t *appl, int index);

klangc_parse_result_t klangc_expr_appl_parse(klangc_input_t *input,
                                             klangc_expr_parse_opt_t epopt,
                                             klangc_expr_t *efunc,
                                             klangc_expr_appl_t **pexpr_appl);

void klangc_expr_appl_print(klangc_output_t *output, int prec,
                            klangc_expr_appl_t *appl);

klangc_bind_result_t klangc_expr_appl_bind(klangc_expr_env_t *env,
                                           klangc_expr_appl_t *appl);
