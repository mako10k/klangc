#pragma once

#include "klangc.h"

klangc_parse_result_t klangc_pattern_parse(klangc_input_t *input,
                                           klangc_pattern_t **ppattern);

void klangc_pattern_print(klangc_output_t *output, int prec,
                          klangc_pattern_t *pattern);

int klangc_pattern_walkvars(klangc_closure_t *closure, klangc_bind_t *bind,
                            klangc_pattern_t *pat,
                            int (*bind_fn)(klangc_closure_t *, const char *,
                                           klangc_bind_t *));
