#pragma once

#include "../../klangc.h"

klangc_parse_result_t
klangc_expr_closure_bare_parse(klangc_input_t *input,
                               klangc_expr_closure_t *upper,
                               klangc_expr_closure_t **pclosure);

void klangc_expr_closure_bare_print(klangc_output_t *output,
                                    klangc_expr_closure_t *closure);
