#pragma once

#include "klangc.h"
#include "klangc_types.h"

klangc_closure_ent_t *klangc_closure_ent_new_bind(klangc_bind_t *bind);

klangc_closure_ent_t *
klangc_closure_ent_new_lambda(klangc_expr_lambda_t *lambda);

int klangc_closure_ent_isbind(klangc_closure_ent_t *ent);

int klangc_closure_ent_islambda(klangc_closure_ent_t *ent);

klangc_bind_t *klangc_closure_ent_get_bind(klangc_closure_ent_t *ent);

klangc_expr_lambda_t *klangc_closure_ent_get_lambda(klangc_closure_ent_t *ent);

klangc_closure_ent_t *klangc_closure_ent_get_next(klangc_closure_ent_t *ent);

void klangc_closure_ent_set_next(klangc_closure_ent_t *ent,
                                 klangc_closure_ent_t *ent_prev);

klangc_parse_result_t klangc_closure_ent_parse(klangc_input_t *input,
                                               klangc_closure_t *closure,
                                               klangc_closure_ent_t **pent);

void klangc_closure_ent_print(klangc_output_t *output,
                              klangc_closure_ent_t *ent);