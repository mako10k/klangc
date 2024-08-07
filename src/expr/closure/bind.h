#pragma once

#include "../../klangc.h"

klangc_expr_closure_bind_t *klangc_expr_closure_bind_new(klangc_pattern_t *pat,
                                                         klangc_expr_t *expr,
                                                         klangc_ipos_t ipos);

klangc_pattern_t *
klangc_expr_closure_bind_get_pat(klangc_expr_closure_bind_t *bind);

klangc_expr_t *
klangc_expr_closure_bind_get_expr(klangc_expr_closure_bind_t *bind);

klangc_ipos_t
klangc_expr_closure_bind_get_ipos(klangc_expr_closure_bind_t *bind);

klangc_parse_result_t
klangc_expr_closure_bind_parse(klangc_input_t *input,
                               klangc_expr_closure_bind_t **pbind);

void klangc_expr_closure_bind_print(klangc_output_t *output,
                                    klangc_expr_closure_bind_t *bind);