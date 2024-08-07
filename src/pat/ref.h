#pragma once

#include "../klangc.h"

klangc_pattern_ref_t *klangc_pattern_ref_new(klangc_ref_t *ref);

klangc_ref_t *klangc_pattern_ref_get_ref(klangc_pattern_ref_t *pattern_ref);

const char *klangc_pattern_ref_get_name(klangc_pattern_ref_t *pattern_ref);

klangc_ipos_t klangc_pattern_ref_get_ipos(klangc_pattern_ref_t *pattern_ref);

int klangc_pattern_ref_is_used(klangc_pattern_ref_t *pattern_ref);

void klangc_pattern_ref_set_used(klangc_pattern_ref_t *pattern_ref);

klangc_parse_result_t
klangc_pattern_ref_parse(klangc_input_t *input,
                         klangc_pattern_ref_t **ppattern_ref);

void klangc_pattern_ref_print(klangc_output_t *output,
                              klangc_pattern_ref_t *pattern_ref);