#pragma once

#include "klangc.h"
#include "klangc_input.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_types.h"

klangc_parse_result_t
klangc_expr_closure_parse(klangc_input_t *input, klangc_closure_t *upper,
                          klangc_expr_closure_t **peclosure);

void klangc_expr_closure_print(klangc_output_t *output,
                               klangc_expr_closure_t *eclosure);

klangc_closure_t *
klangc_expr_closure_get_closure(klangc_expr_closure_t *eclosure);
