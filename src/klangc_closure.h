#pragma once

#include "klangc.h"
#include "klangc_input.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_types.h"

typedef int (*klangc_bare_closure_walk_func_t)(klangc_closure_t *closure,
                                               klangc_closure_ent_t *ent,
                                               void *data);

typedef int (*klangc_bare_closure_walk_bind_func_t)(klangc_closure_t *closure,
                                                    klangc_bind_t *bind,
                                                    void *data);

typedef int (*klangc_bare_closure_walk_lambda_func_t)(klangc_closure_t *closure,
                                                      klangc_lambda_t *lambda,
                                                      void *data);

klangc_closure_t *klangc_closure_new(klangc_ipos_t ipos,
                                     klangc_closure_t *upper);

klangc_parse_result_t klangc_closure_parse(klangc_input_t *input,
                                           klangc_closure_t *upper,
                                           klangc_closure_t **pclosure);

void klangc_closure_print(klangc_output_t *output, klangc_closure_t *eclosure);

int klangc_closure_put_bind(klangc_closure_t *closure, klangc_ref_t *ref,
                            klangc_bind_t *bind);

int klangc_closure_get_bind(klangc_closure_t *closure, klangc_ref_t *ref,
                            klangc_bind_t **pbind, klangc_closure_t **pclosure);

int klangc_closure_walk(klangc_closure_t *closure,
                        klangc_bare_closure_walk_func_t func, void *data);

int klangc_closure_walk_bind(klangc_closure_t *closure,
                             klangc_bare_closure_walk_bind_func_t func,
                             void *data);

int klangc_closure_walk_lambda(klangc_closure_t *closure,
                               klangc_bare_closure_walk_lambda_func_t func,
                               void *data);

void klangc_closure_set_ent_first(klangc_closure_t *closure,
                                  klangc_closure_ent_t *ent);