#pragma once

#include "klangc.h"
#include "pat.h"

// *******************************
// Pattern Algebra.
// *******************************
// -------------------------------
// Constructors.
// -------------------------------
/**
 * Make a new pattern algebra.
 * @param constr Constructor.
 * @return New pattern algebra.
 */
klangc_pat_alge_t *klangc_pat_alge_new(klangc_symbol_t *constr);

/**
 * Add an argument to a pattern algebra.
 * @param palge Pattern algebra.
 * @param arg Argument.
 */
void klangc_pat_alge_add_arg(klangc_pat_alge_t *palge, klangc_pat_t *arg);

// -------------------------------
// Accessors.
// -------------------------------
/**
 * Get the constructor of a pattern algebra.
 * @param palge Pattern algebra.
 * @return Constructor.
 */
klangc_symbol_t *klangc_pat_alge_get_constr(klangc_pat_alge_t *palge);

/**
 * Get the number of arguments of a pattern algebra.
 * @param palge Pattern algebra.
 * @return Number of arguments.
 */
unsigned int klangc_pat_alge_get_argc(klangc_pat_alge_t *palge);

/**
 * Get an argument of a pattern algebra.
 * @param palge Pattern algebra.
 * @param index Index.
 * @return Argument.
 */
klangc_pat_t *klangc_pat_alge_get_arg(klangc_pat_alge_t *palge, int index);

// -------------------------------
// Printers.
// -------------------------------
/**
 * Print a pattern algebra.
 * @param output Output.
 * @param prec Precedence.
 * @param palge Pattern algebra.
 */
void klangc_pat_alge_print(klangc_output_t *output, int prec,
                           klangc_pat_alge_t *palge);

// -------------------------------
// Parsers.
// -------------------------------
/**
 * Parse a pattern algebra.
 * @param input Input.
 * @param ppopts Parse options.
 * @param ppalge Pattern algebra.
 * @return Parse result.
 */
klangc_parse_result_t klangc_pat_alge_parse(klangc_input_t *input,
                                            klangc_pat_parse_opt_t ppopts,
                                            klangc_pat_alge_t **ppalge);

// -------------------------------
// Binders.
// -------------------------------
/**
 * Bind a pattern algebra.
 * @param env Environment.
 * @param palge Pattern algebra.
 * @param target Target.
 * @return Bind result.
 */
klangc_bind_result_t klangc_pat_alge_bind(klangc_expr_env_t *env,
                                          klangc_pat_alge_t *palge,
                                          klangc_expr_ref_target_t *target);