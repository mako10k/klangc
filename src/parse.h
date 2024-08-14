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
 * parse a expected character
 * @param input input
 * @param c expected character
 * @param pc character to store the parsed character
 * @return parse result
 */
klangc_parse_result_t klangc_expect(klangc_input_t *input, int c, int *pc);
