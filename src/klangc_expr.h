#pragma once

#include "klangc.h"

klangc_parse_result_t klangc_expr_parse(klangc_input_t *input,
                                        klangc_bare_closure_t *enclosed_by,
                                        klangc_expr_t **pexpr);

void klangc_expr_print(klangc_output_t *output, int prec, klangc_expr_t *expr);

int klangc_expr_bind(klangc_bare_closure_t *closure, klangc_expr_t *expr);

int klangc_expr_bind_for_walk(klangc_bare_closure_t *closure, klangc_bind_t *bind,
                              void *data);

int klangc_expr_check_unbound_for_walk(klangc_bare_closure_t *closure,
                                       klangc_bind_t *bind, void *data);

klangc_ipos_t klangc_expr_get_ipos(klangc_expr_t *expr);