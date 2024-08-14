#pragma once

#include "klangc.h"

// *******************************
// Program.
// *******************************
// -------------------------------
// Constructors.
// -------------------------------
/**
 * Make a new program.
 * @return New program.
 */
klangc_program_t *klangc_program_new(klangc_expr_closure_t *closure);

klangc_parse_result_t klangc_program_parse(klangc_input_t *input,
                                           klangc_program_t **pprogram);

void klangc_program_print(klangc_output_t *output, klangc_program_t *program);

klangc_bind_result_t klangc_program_bind(klangc_expr_env_t *env,
                                         klangc_program_t *program);