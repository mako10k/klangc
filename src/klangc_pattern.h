#pragma once

#include "klangc.h"

typedef int (*klangc_pattern_foreach_ref_func_t)(klangc_pattern_ref_t *ref, void *data);

klangc_parse_result_t klangc_pattern_parse(klangc_input_t *input,
                                           klangc_pattern_t **ppattern);

void klangc_pattern_print(klangc_output_t *output, int prec,
                          klangc_pattern_t *pattern);

int klangc_pattern_foreach_ref(klangc_pattern_t *pat,
                               klangc_pattern_foreach_ref_func_t bind_fn,
                               void *data);
