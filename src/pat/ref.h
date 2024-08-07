#pragma once

#include "../klangc.h"

klangc_pat_ref_t *klangc_pat_ref_new(klangc_ref_t *ref);

klangc_ref_t *klangc_pat_ref_get_ref(klangc_pat_ref_t *pat_ref);

const char *klangc_pat_ref_get_name(klangc_pat_ref_t *pat_ref);

klangc_ipos_t klangc_pat_ref_get_ipos(klangc_pat_ref_t *pat_ref);

int klangc_pat_ref_is_used(klangc_pat_ref_t *pat_ref);

void klangc_pat_ref_set_used(klangc_pat_ref_t *pat_ref);

klangc_parse_result_t klangc_pat_ref_parse(klangc_input_t *input,
                                           klangc_pat_ref_t **ppat_ref);

void klangc_pat_ref_print(klangc_output_t *output, klangc_pat_ref_t *pat_ref);