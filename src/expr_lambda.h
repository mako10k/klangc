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
 * @param arg Argument pattern.
 * @param body Body expression.
 * @param next Next lambda expression.
 * @return New lambda expression.
 */
klangc_expr_lambda_t *klangc_expr_lambda_new(klangc_pat_t *arg,
                                             klangc_expr_t *body,
                                             klangc_expr_lambda_t *next);

// -------------------------------
// Accessors.
// -------------------------------
/**
 * Get the argument pattern of a lambda expression.
 * @param lambda Lambda expression.
 * @return Argument pattern.
 */
klangc_pat_t *klangc_expr_lambda_get_arg(klangc_expr_lambda_t *lambda);

/**
 * Get the body expression of a lambda expression.
 * @param lambda Lambda expression.
 * @return Body expression.
 */
klangc_expr_t *klangc_expr_lambda_get_body(klangc_expr_lambda_t *lambda);

/**
 * Get the next lambda expression of a lambda expression.
 * @param lambda Lambda expression.
 * @return Next lambda expression.
 */
klangc_expr_lambda_t *klangc_expr_lambda_get_next(klangc_expr_lambda_t *lambda);

// -------------------------------
// Parsers.
// -------------------------------
/**
 * Parse a lambda expression.
 * @param input Input stream.
 * @param plambda Pointer to the result.
 * @return Parsing result.
 */
klangc_parse_result_t klangc_expr_lambda_parse(klangc_input_t *input,
                                               klangc_expr_lambda_t **plambda);

// -------------------------------
// Printers.
// -------------------------------
/**
 * Print a lambda expression.
 * @param output Output stream.
 * @param prec Precedence.
 * @param lambda Lambda expression.
 */
void klangc_expr_lambda_print(klangc_output_t *output, int prec,
                              klangc_expr_lambda_t *lambda);

// -------------------------------
// Binders.
// -------------------------------
/**
 * Bind a lambda expression.
 * @param env Environment.
 * @param lambda Lambda expression.
 * @return Binding result.
 */
klangc_bind_result_t klangc_expr_lambda_bind(klangc_expr_env_t *env,
                                             klangc_expr_lambda_t *lambda);
