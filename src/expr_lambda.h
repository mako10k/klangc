#pragma once

#include "klangc.h"

// *******************************
// Lambda expression.
// *******************************
// -------------------------------
// Constructors.
// -------------------------------
/**
 * Make a new lambda expression.
 * @param pat_arg Argument pattern.
 * @param expr_body Body expression.
 * @param elam_next Next lambda expression.
 * @return New lambda expression.
 */
klangc_expr_lambda_t *klangc_expr_lambda_new(klangc_pat_t *pat_arg,
                                             klangc_expr_t *expr_body,
                                             klangc_expr_lambda_t *elam_next);

// -------------------------------
// Accessors.
// -------------------------------
/**
 * Get the number of parameters of a lambda expression.
 * @param elam Lambda expression.
 * @return number of parameters.
 */
unsigned int klangc_expr_lambda_get_paramc(klangc_expr_lambda_t *elam);

/**
 * Get the parameter pattern of a lambda expression.
 * @param elam Lambda expression.
 * @param index Index.
 * @return Parameter pattern.
 */
klangc_pat_t *klangc_expr_lambda_get_params(klangc_expr_lambda_t *elam,
                                            unsigned int index);

/**
 * Get the body expression of a lambda expression.
 * @param elam Lambda expression.
 * @return Body expression.
 */
klangc_expr_t *klangc_expr_lambda_get_body(klangc_expr_lambda_t *elam);

/**
 * Get the next lambda expression of a lambda expression.
 * @param elam Lambda expression.
 * @return Next lambda expression.
 */
klangc_expr_lambda_t *klangc_expr_lambda_get_next(klangc_expr_lambda_t *elam);

// -------------------------------
// Parsers.
// -------------------------------
/**
 * Parse a lambda expression.
 * @param input Input stream.
 * @param pelam Pointer to the result.
 * @return Parsing result.
 */
klangc_parse_result_t klangc_expr_lambda_parse(klangc_input_t *input,
                                               klangc_expr_lambda_t **pelam);

// -------------------------------
// Printers.
// -------------------------------
/**
 * Print a lambda expression.
 * @param output Output stream.
 * @param prec Precedence.
 * @param elam Lambda expression.
 */
void klangc_expr_lambda_print(klangc_output_t *output, int prec,
                              klangc_expr_lambda_t *elam);

// -------------------------------
// Binders.
// -------------------------------
/**
 * Bind a lambda expression.
 * @param env Environment.
 * @param elam Lambda expression.
 * @return Binding result.
 */
klangc_bind_result_t klangc_expr_lambda_bind(klangc_expr_env_t *env,
                                             klangc_expr_lambda_t *elam);
