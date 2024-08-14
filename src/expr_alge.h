#pragma once

#include "expr.h"
#include "klangc.h"

// *******************************
// Algebraic expression.
// *******************************

// -------------------------------
// Constructors.
// -------------------------------
/**
 * Make a new algebraic expression without arguments.
 * @param constr Constructor symbol.
 * @return New algebraic expression.
 */
klangc_expr_alge_t *klangc_expr_alge_new(const klangc_symbol_t *constr);

/**
 * Add an argument to an algebraic expression.
 * @param ealge Algebraic expression to be added to.
 * @param arg Argument expression.
 */
void klangc_expr_alge_add_arg(klangc_expr_alge_t *ealge, klangc_expr_t *arg);

// -------------------------------
// Accessors.
// -------------------------------
/**
 * Get the constructor symbol of an algebraic expression.
 * @param ealge Algebraic expression.
 * @return Constructor symbol.
 */
const klangc_symbol_t *klangc_expr_alge_get_constr(klangc_expr_alge_t *ealge);

/**
 * Get the number of arguments of an algebraic expression.
 * @param ealge Algebraic expression.
 * @return Number of arguments.
 */
unsigned int klangc_expr_alge_get_argc(klangc_expr_alge_t *ealge);

/**
 * Get the argument at a given index of an algebraic expression.
 * @param ealge Algebraic expression.
 * @param index Index of the argument.
 * @return Argument expression.
 */
klangc_expr_t *klangc_expr_alge_get_arg(klangc_expr_alge_t *ealge, int index);

// -------------------------------
// Parsers.
// -------------------------------
/**
 * Parse an algebraic expression.
 * @param input Input stream.
 * @param epopt Expression parsing options.
 * @param pealge Pointer to the result.
 * @return Parsing result.
 */
klangc_parse_result_t klangc_expr_alge_parse(klangc_input_t *input,
                                             klangc_expr_parse_opt_t epopt,
                                             klangc_expr_alge_t **pealge);

// -------------------------------
// Printers.
// -------------------------------
/**
 * Print an algebraic expression.
 * @param output Output stream.
 * @param prec Precedence of the parent expression.
 * @param ealge Algebraic expression.
 */
void klangc_expr_alge_print(klangc_output_t *output, int prec,
                            klangc_expr_alge_t *ealge);

// -------------------------------
// Binders.
// -------------------------------
/**
 * Bind an algebraic expression.
 * @param upper Environment of the parent expression.
 * @param ealge Algebraic expression.
 * @return Binding result.
 */
klangc_bind_result_t klangc_expr_alge_bind(klangc_expr_env_t *upper,
                                           klangc_expr_alge_t *ealge);
