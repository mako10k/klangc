#pragma once

#include "klangc.h"

klangc_symbol_t *klangc_symbol_new(const klangc_str_t *name);

const klangc_str_t *klangc_symbol_get_name(klangc_symbol_t *symbol);

klangc_parse_result_t klangc_symbol_parse(klangc_input_t *input,
                                          klangc_symbol_t **psymbol);

void klangc_symbol_print(klangc_output_t *output, klangc_symbol_t *symbol);
