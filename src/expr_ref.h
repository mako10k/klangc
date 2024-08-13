#pragma once

#include "klangc.h"

// *******************************
// Reference expression.
// *******************************
// -------------------------------
// Constructors.
// -------------------------------
/**
 * Make a new reference expression.
 * @param ref Reference.
 * @return New reference expression.
 */
klangc_expr_ref_t *klangc_expr_ref_new(klangc_ref_t *ref);

/**
 * Set the target of a reference expression.
 * @param eref Reference expression.
 * @param target Target.
 */
void klangc_expr_ref_set_target(klangc_expr_ref_t *eref,
                                klangc_expr_ref_target_t *target);

// -------------------------------
// Accessors.
// -------------------------------
/**
 * Get the reference of a reference expression.
 * @param eref Reference expression.
 * @return Reference.
 */
klangc_ref_t *klangc_expr_ref_get_ref(klangc_expr_ref_t *eref);

/**
 * Get the target of a reference expression.
 * @param eref Reference expression.
 * @return Target.
 */
klangc_expr_ref_target_t *klangc_expr_ref_get_target(klangc_expr_ref_t *eref);

// -------------------------------
// Parsers.
// -------------------------------
/**
 * Parse a reference expression.
 * @param input Input stream.
 * @param eref Pointer to the result.
 * @return Parsing result.
 */
klangc_parse_result_t klangc_expr_ref_parse(klangc_input_t *input,
                                            klangc_expr_ref_t **peref);

// -------------------------------
// Printers.
// -------------------------------
/**
 * Print a reference expression.
 * @param output Output stream.
 * @param eref Reference expression.
 */
void klangc_expr_ref_print(klangc_output_t *output, klangc_expr_ref_t *eref);

// -------------------------------
// Binders.
// -------------------------------
klangc_bind_result_t klangc_expr_ref_bind(klangc_expr_env_t *eenv,
                                          klangc_expr_ref_t *eref);
