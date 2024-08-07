#pragma once

#include "../klangc.h"

klangc_expr_ref_t *klangc_expr_ref_new(klangc_ref_t *ref);

klangc_ref_t *klangc_expr_ref_get_ref(klangc_expr_ref_t *expr);

klangc_symbol_t *klangc_expr_ref_get_symbol(klangc_expr_ref_t *expr);

char *klangc_expr_ref_get_name(klangc_expr_ref_t *expr);

klangc_ipos_t klangc_expr_ref_get_ipos(klangc_expr_ref_t *expr);

klangc_expr_closure_bind_t *klangc_expr_ref_get_bind(klangc_expr_ref_t *expr);

void klangc_expr_ref_set_bind(klangc_expr_ref_t *expr,
                              klangc_expr_closure_bind_t *bind);

klangc_parse_result_t klangc_expr_ref_parse(klangc_input_t *input,
                                            klangc_expr_ref_t **pexpr_ref);

void klangc_expr_ref_print(klangc_output_t *output,
                           klangc_expr_ref_t *expr_ref);