#pragma once

#include "../../klangc.h"

klangc_parse_result_t klangc_bare_closure_parse(klangc_input_t *input,
                                                klangc_closure_t *upper,
                                                klangc_closure_t **pclosure);

void klangc_bare_closure_print(klangc_output_t *output,
                               klangc_closure_t *closure);
