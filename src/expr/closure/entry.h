#pragma once

#include "../../klangc.h"

klangc_expr_closure_entry_t *
klangc_expr_closure_entry_new_bind(klangc_expr_closure_bind_t *bind);

klangc_expr_closure_entry_t *
klangc_expr_closure_entry_new_lambda(klangc_lambda_t *lambda);

int klangc_expr_closure_entry_isbind(klangc_expr_closure_entry_t *ent);

int klangc_expr_closure_entry_islambda(klangc_expr_closure_entry_t *ent);

klangc_expr_closure_bind_t *
klangc_expr_closure_entry_get_bind(klangc_expr_closure_entry_t *ent);

klangc_lambda_t *
klangc_expr_closure_entry_get_lambda(klangc_expr_closure_entry_t *ent);

klangc_expr_closure_entry_t *
klangc_expr_closure_entry_get_next(klangc_expr_closure_entry_t *ent);

void klangc_expr_closure_entry_set_next(klangc_expr_closure_entry_t *ent,
                                        klangc_expr_closure_entry_t *ent_prev);

klangc_parse_result_t
klangc_expr_closure_entry_parse(klangc_input_t *input,
                                klangc_closure_t *closure,
                                klangc_expr_closure_entry_t **pent);

void klangc_expr_closure_entry_print(klangc_output_t *output,
                                     klangc_expr_closure_entry_t *ent);