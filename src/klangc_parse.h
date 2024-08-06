#pragma once

#include "klangc.h"
#include "klangc_input.h"

klangc_parse_result_t klangc_int_parse(klangc_input_t *input, int *pintval);

klangc_parse_result_t klangc_string_parse(klangc_input_t *input,
                                          char **pstrval);

int klangc_expect(klangc_input_t *input, int c, int *pc);
