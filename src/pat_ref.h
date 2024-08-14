#pragma once

#include "klangc.h"

// *******************************
// Pattern Reference.
// *******************************
// -------------------------------
// Constructors.
// -------------------------------
/**
 * Make a new pattern reference.
 * @param sym Reference.
 * @return New pattern reference.
 */
klangc_pat_ref_t *klangc_pat_ref_new(const klangc_symbol_t *sym);

// -------------------------------
// Accessors.
// -------------------------------
const klangc_symbol_t *klangc_pat_ref_get_symbol(klangc_pat_ref_t *pref);

// -------------------------------
// Parsers.
// -------------------------------
/**
 * Parse a pattern reference.
 * @param input Input.
 * @param pref Pattern reference.
 * @return Parse result.
 */
klangc_parse_result_t klangc_pat_ref_parse(klangc_input_t *input,
                                           klangc_pat_ref_t **pref);

// -------------------------------
// Printers.
// -------------------------------
/**
 * Print a pattern reference.
 * @param output Output.
 * @param pref Pattern reference.
 */
void klangc_pat_ref_print(klangc_output_t *output, klangc_pat_ref_t *pref);

// -------------------------------
// Binders.
// -------------------------------
/**
 * Bind a pattern reference.
 * @param env Environment.
 * @param pref Pattern reference.
 * @param target Target.
 * @return Bind result.
 */
klangc_bind_result_t klangc_pat_ref_bind(klangc_expr_env_t *env,
                                         klangc_pat_ref_t *pref,
                                         klangc_expr_ref_target_t *target);