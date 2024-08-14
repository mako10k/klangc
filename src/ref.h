#pragma once

#include "klangc.h"

klangc_parse_result_t klangc_ref_parse(klangc_input_t *input,
                                       const klangc_symbol_t **psym);

void klangc_ref_print(klangc_output_t *output, const klangc_symbol_t *sym);