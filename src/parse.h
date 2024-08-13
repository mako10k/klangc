#pragma once

#include "input.h"
#include "klangc.h"

// *******************************
// Parse.
// *******************************
// -------------------------------
// Parsers.
// -------------------------------
/**
 * parse an integer
 * @param input input
 * @param pintval integer to store the parsed integer
 */
klangc_parse_result_t klangc_int_parse(klangc_input_t *input, int *pintval);

/**
 * parse a string
 * @param input input
 * @param pstrval string to store the parsed string
 */
klangc_parse_result_t klangc_string_parse(klangc_input_t *input,
                                          const char **pstrval);

/**
 * parse a expected character
 * @param input input
 * @param c expected character
 * @param pcval character to store the parsed character
 */
int klangc_expect(klangc_input_t *input, int c, int *pc);
