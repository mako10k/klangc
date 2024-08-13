#pragma once

#include "expr.h"
#include "klangc.h"

// *******************************
// Function application expression.
// *******************************
// -------------------------------
// Constructors.
// -------------------------------
/**
 * Make a new function application expression.
 * @param func Function expression.
 * @return New function application expression.
 */
klangc_expr_appl_t *klangc_expr_appl_new(klangc_expr_t *func);

/**
 * Add an argument to a function application expression.
 * @param appl Function application expression to be added to.
 * @param arg Argument expression.
 */
void klangc_expr_appl_add_arg(klangc_expr_appl_t *appl, klangc_expr_t *arg);

// -------------------------------
// Accessors.
// -------------------------------
/**
 * Get the function expression of a function application expression.
 * @param appl Function application expression.
 * @return Function expression.
 */
klangc_expr_t *klangc_expr_appl_get_func(klangc_expr_appl_t *appl);

/**
 * Get the number of arguments of a function application expression.
 * @param appl Function application expression.
 * @return Number of arguments.
 */
unsigned int klangc_expr_appl_get_argc(klangc_expr_appl_t *appl);

/**
 * Get the argument at a given index of a function application expression.
 * @param appl Function application expression.
 * @param index Index of the argument.
 * @return Argument expression.
 */
klangc_expr_t *klangc_expr_appl_get_arg(klangc_expr_appl_t *appl, int index);

// -------------------------------
// Parsers.
// -------------------------------
/**
 * Parse a function application expression.
 * @param input Input stream.
 * @param epopt Expression parsing options.
 * @param efunc Preparsed function expression.
 * @param pappl Pointer to the result.
 * @return Parsing result.
 */
klangc_parse_result_t klangc_expr_appl_parse(klangc_input_t *input,
                                             klangc_expr_parse_opt_t epopt,
                                             klangc_expr_t *efunc,
                                             klangc_expr_appl_t **pappl);

// -------------------------------
// Printers.
// -------------------------------
/**
 * Print a function application expression.
 * @param output Output stream.
 * @param prec Precedence level.
 * @param appl Function application expression.
 */
void klangc_expr_appl_print(klangc_output_t *output, int prec,
                            klangc_expr_appl_t *appl);

// -------------------------------
// Binders.
// -------------------------------
/**
 * Bind a function application expression.
 * @param env Expression environment.
 * @param appl Function application expression.
 * @return Binding result.
 */
klangc_bind_result_t klangc_expr_appl_bind(klangc_expr_env_t *env,
                                           klangc_expr_appl_t *appl);
