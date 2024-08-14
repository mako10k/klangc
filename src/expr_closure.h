#pragma once

#include "klangc.h"

// *******************************
// Closure expression.
// *******************************
// -------------------------------
// Constructors.
// -------------------------------
/**
 * Make a new closure expression.
 * @param expr Expression.
 * @param bind Binding.
 * @return New closure expression.
 */
klangc_expr_closure_t *klangc_expr_closure_new(klangc_expr_t *expr,
                                               klangc_bind_t *bind);

// -------------------------------
// Accessors.
// -------------------------------
/**
 * Get the expression of a closure.
 * @param closure Closure.
 * @return Expression.
 */
klangc_expr_t *klangc_expr_closure_get_expr(klangc_expr_closure_t *closure);

/**
 * Get the binding of a closure.
 * @param closure Closure.
 * @return Binding.
 */
klangc_bind_t *klangc_expr_closure_get_bind(klangc_expr_closure_t *closure);

// -------------------------------
// Parsers.
// -------------------------------
/**
 * Parse a closure expression without surrounding braces.
 * @param input Input stream.
 * @param pclosure Pointer to the result.
 * @return Parsing result.
 */
klangc_parse_result_t
klangc_expr_closure_parse_nobrace(klangc_input_t *input,
                                  klangc_expr_closure_t **pclosure);

/**
 * Parse a closure expression.
 * @param input Input stream.
 * @param pclosure Pointer to the result.
 * @return Parsing result.
 */
klangc_parse_result_t
klangc_expr_closure_parse(klangc_input_t *input,
                          klangc_expr_closure_t **pclosure);

// -------------------------------
// Printers.
// -------------------------------
/**
 * Print a closure expression.
 * @param output Output stream.
 * @param eclosure Closure expression.
 */
void klangc_expr_closure_print(klangc_output_t *output,
                               klangc_expr_closure_t *eclosure);

/**
 * Print a closure expression without surrounding braces.
 * @param output Output stream.
 * @param eclosure Closure expression.
 */
void klangc_expr_closure_print_nobrace(klangc_output_t *output,
                                       klangc_expr_closure_t *eclosure);

// -------------------------------
// Binders.
// -------------------------------
/**
 * Bind a closure expression.
 * @param env Environment.
 * @param closure Closure.
 * @return Binding result.
 */
klangc_bind_result_t klangc_expr_closure_bind(klangc_expr_env_t *env,
                                              klangc_expr_closure_t *closure);
