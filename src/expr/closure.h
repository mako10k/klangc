#pragma once

#include "../klangc.h"

typedef int (*klangc_expr_closure_each_ent_func_t)(
    klangc_expr_closure_t *closure, klangc_expr_closure_entry_t *ent,
    void *data);

typedef int (*klangc_expr_closure_each_bind_func_t)(
    klangc_expr_closure_t *closure, klangc_expr_closure_bind_t *bind,
    void *data);

typedef int (*klangc_expr_closure_each_lambda_func_t)(
    klangc_expr_closure_t *closure, klangc_expr_lambda_t *lambda, void *data);

klangc_expr_closure_t *klangc_expr_closure_new(klangc_ipos_t ipos,
                                               klangc_expr_closure_t *upper);

klangc_parse_result_t
klangc_expr_closure_parse(klangc_input_t *input, klangc_expr_closure_t *upper,
                          klangc_expr_closure_t **pclosure);

void klangc_expr_closure_print(klangc_output_t *output,
                               klangc_expr_closure_t *eclosure);

int klangc_expr_closure_put_bind(klangc_expr_closure_t *closure,
                                 klangc_ref_t *ref,
                                 klangc_expr_closure_bind_t *bind);

int klangc_expr_closure_get_bind(klangc_expr_closure_t *closure,
                                 klangc_ref_t *ref,
                                 klangc_expr_closure_bind_t **pbind,
                                 klangc_expr_closure_t **pclosure);

int klangc_expr_closure_foreach_ent(klangc_expr_closure_t *closure,
                                    klangc_expr_closure_each_ent_func_t func,
                                    void *data);

int klangc_expr_closure_foreach_bind(klangc_expr_closure_t *closure,
                                     klangc_expr_closure_each_bind_func_t func,
                                     void *data);

int klangc_expr_closure_foreach_lambda(
    klangc_expr_closure_t *closure, klangc_expr_closure_each_lambda_func_t func,
    void *data);

klangc_expr_closure_entry_t *
klangc_expr_closure_get_ent_first(klangc_expr_closure_t *closure);

void klangc_expr_closure_set_ent_first(klangc_expr_closure_t *closure,
                                       klangc_expr_closure_entry_t *ent);

int klangc_expr_closure_bind(klangc_expr_closure_t *closure);

int klangc_expr_closure_check_unbound(klangc_output_t *output,
                                      klangc_expr_closure_t *closure);