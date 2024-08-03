#ifndef __KLANGC_DEF_H__
#define __KLANGC_DEF_H__

#include "klangc.h"

#include "klangc_input.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_types.h"

klangc_parse_result_t klangc_closure_parse(klangc_input_t *input,
                                           klangc_closure_t *upper,
                                           klangc_closure_t **pclosure);

void klangc_closure_print(klangc_output_t *output, klangc_closure_t *closure);

int klangc_closure_makeref_bind(klangc_closure_t *closure, const char *name,
                                klangc_bind_t *bind);

klangc_closure_t *klangc_closure_new(klangc_ipos_t ipos,
                                     klangc_closure_t *upper);

int klangc_closure_get_bind(klangc_closure_t *closure, const char *name,
                            klangc_bind_t **pbind, klangc_closure_t **pclosure);

int klangc_closure_walk_bind(klangc_closure_t *closure,
                             int (*func)(klangc_closure_t *closure,
                                         klangc_pattern_t *pat,
                                         klangc_expr_t *expr, void *data),
                             void *data);

#endif // __KLANGC_DEF_H__