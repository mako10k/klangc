#pragma once

#include "klangc.h"

// *******************************
// Binding sub-expression.
// *******************************
// -------------------------------
// Constructors.
// -------------------------------
/**
 * Make a new binding sub-expression.
 * @param pat Reference pattern.
 * @param expr Referenced expression.
 * @param next Next binding.
 * @param ipos Position information.
 * @return New binding sub-expression.
 */
klangc_bind_t *klangc_bind_new(klangc_pat_t *pat, klangc_expr_t *expr,
                               klangc_bind_t *next, klangc_ipos_t ipos);

// -------------------------------
// Accessors.
// -------------------------------
/**
 * Get the reference pattern of a binding sub-expression.
 * @param bind Binding sub-expression.
 * @return Reference pattern.
 */
klangc_pat_t *klangc_bind_get_pat(klangc_bind_t *bind);

/**
 * Get the referenced expression of a binding sub-expression.
 * @param bind Binding sub-expression.
 * @return Referenced expression.
 */
klangc_expr_t *klangc_bind_get_expr(klangc_bind_t *bind);

/**
 * Get the next binding sub-expression of a binding sub-expression.
 * @param bind Binding sub-expression.
 * @return Next binding sub-expression.
 */
klangc_bind_t *klangc_bind_get_next(klangc_bind_t *bind);

/**
 * Get the position information of a binding sub-expression.
 * @param bind Binding sub-expression.
 * @return Position information.
 */
klangc_ipos_t klangc_bind_get_ipos(klangc_bind_t *bind);

// -------------------------------
// Parsers.
// -------------------------------
/**
 * Parse a binding sub-expression.
 * @param input Input stream.
 * @param pbind Pointer to the result.
 * @return Parsing result.
 */
klangc_parse_result_t klangc_bind_parse(klangc_input_t *input,
                                        klangc_bind_t **pbind);

// -------------------------------
// Printers.
// -------------------------------
/**
 * Print a binding sub-expression.
 * @param output Output stream.
 * @param bind Binding sub-expression.
 */
void klangc_bind_print(klangc_output_t *output, klangc_bind_t *bind);

// -------------------------------
// Binders.
// -------------------------------
/**
 * Bind a binding sub-expression.
 * @param env Environment of the parent expression.
 * @param bind Binding sub-expression.
 * @return Binding result.
 */
klangc_bind_result_t klangc_bind_bind(klangc_expr_env_t *env,
                                      klangc_bind_t *bind);
