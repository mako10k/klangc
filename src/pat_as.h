#pragma once

#include "klangc.h"

// *******************************
// Pattern As.
// *******************************
// -------------------------------
// Constructors.
// -------------------------------
/**
 * Make a new pattern as.
 * @param pref Reference.
 * @param pat Pattern.
 * @return New pattern as.
 */
klangc_pat_as_t *klangc_pat_as_new(klangc_pat_ref_t *pref, klangc_pat_t *pat);

// -------------------------------
// Accessors.
// -------------------------------
/**
 * Get the reference of a pattern as.
 * @param pas Pattern as.
 * @return Reference.
 */
klangc_pat_ref_t *klangc_pat_as_get_ref(klangc_pat_as_t *pas);

/**
 * Get the pattern of a pattern as.
 * @param pas Pattern as.
 * @return Pattern.
 */
klangc_pat_t *klangc_pat_as_get_pat(klangc_pat_as_t *pas);

// -------------------------------
// Printers.
// -------------------------------
/**
 * Print a pattern as.
 * @param output Output.
 * @param pas Pattern as.
 */
void klangc_pat_as_print(klangc_output_t *output, klangc_pat_as_t *pas);

// -------------------------------
// Binders.
// -------------------------------
klangc_bind_result_t klangc_pat_as_bind(klangc_expr_env_t *env,
                                        klangc_pat_as_t *pas,
                                        klangc_expr_ref_target_t *target);