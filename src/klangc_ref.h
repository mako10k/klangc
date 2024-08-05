#pragma once

#include "klangc.h"
#include "klangc_parse.h"
#include "klangc_types.h"

klangc_ref_t *klangc_ref_new(klangc_symbol_t *symbol);

void klangc_ref_free(klangc_ref_t *ref);

klangc_symbol_t *klangc_ref_get_symbol(klangc_ref_t *ref);

char *klangc_ref_get_name(klangc_ref_t *ref);

klangc_parse_result_t klangc_ref_parse(klangc_input_t *input,
                                       klangc_ref_t **pref);

void klangc_ref_print(klangc_output_t *output, klangc_ref_t *ref);