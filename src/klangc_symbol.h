#pragma once

#include "klangc.h"
#include "klangc_input.h"
#include "klangc_parse.h"
#include "klangc_types.h"

klangc_symbol_t *klangc_symbol_new(const char *name);

void klangc_symbol_free(klangc_symbol_t *symbol);

char *klangc_symbol_get_name(klangc_symbol_t *symbol);

klangc_parse_result_t klangc_symbol_parse(klangc_input_t *input,
                                          klangc_symbol_t **psymbol);

void klangc_symbol_print(klangc_output_t *output, klangc_symbol_t *symbol);