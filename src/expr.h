#pragma once

#include "klangc.h"
#include "types.h"

// *******************************
// Expressions.
// *******************************
// -------------------------------
// Types.
// -------------------------------
/** Expression Type */
typedef enum klangc_expr_type {
  /** Algebratic Expression */
  KLANGC_ETYPE_ALGE,
  /** Reference Expression */
  KLANGC_ETYPE_REF,
  /** Application Expression */
  KLANGC_ETYPE_APPL,
  /** Integer Expression */
  KLANGC_ETYPE_INT,
  /** String Expression */
  KLANGC_ETYPE_STRING,
  /** Lambda Expression */
  KLANGC_ETYPE_LAMBDA,
  /** Closure Expression */
  KLANGC_ETYPE_CLOSURE,
} klangc_expr_type_t;

/** Parse options. */
typedef enum klangc_expr_parse_opt {
  /** parse normally */
  KLANGC_EXPR_PARSE_NORMAL = 0,
  /** do not parse application */
  KLANGC_EXPR_PARSE_NOAPPL = 1,
} klangc_expr_parse_opt_t;

// -------------------------------
// Constructors.
// -------------------------------
/**
 * Create a new algebraic expression.
 * @param ealge algebraic expression
 * @param ipos position
 * @return algebraic expression
 */
klangc_expr_t *klangc_expr_new_alge(klangc_expr_alge_t *ealge,
                                    klangc_ipos_t ipos);

/**
 * Create a new reference expression.
 * @param eref reference expression
 * @param ipos position
 * @return reference expression
 */
klangc_expr_t *klangc_expr_new_ref(klangc_expr_ref_t *eref, klangc_ipos_t ipos);

/**
 * Create a new application expression.
 * @param eappl application expression
 * @param ipos position
 * @return application expression
 */
klangc_expr_t *klangc_expr_new_appl(klangc_expr_appl_t *eappl,
                                    klangc_ipos_t ipos);

/**
 * Create a new integer expression.
 * @param intval integer value
 * @param ipos position
 * @return integer expression
 */
klangc_expr_t *klangc_expr_new_int(int intval, klangc_ipos_t ipos);

/**
 * Create a new string expression.
 * @param strval string value
 * @param ipos position
 * @return string expression
 */
klangc_expr_t *klangc_expr_new_str(const klangc_str_t *strval,
                                   klangc_ipos_t ipos);

/**
 * Create a new lambda expression.
 * @param elambda lambda expression
 * @param ipos position
 * @return lambda expression
 */
klangc_expr_t *klangc_expr_new_lambda(klangc_expr_lambda_t *elambda,
                                      klangc_ipos_t ipos);

/**
 * Create a new closure expression.
 * @param eclosure closure expression
 * @param ipos position
 * @return closure expression
 */
klangc_expr_t *klangc_expr_new_closure(klangc_expr_closure_t *eclosure,
                                       klangc_ipos_t ipos);

// -------------------------------
// Accessors.
// -------------------------------
/**
 * Get the type of an expression.
 * @param expr expression
 * @return type
 */
klangc_expr_type_t klangc_expr_get_type(klangc_expr_t *expr);

/**
 * Get the algebraic expression of an expression.
 * @param expr expression
 * @return algebraic expression
 */
klangc_expr_alge_t *klangc_expr_get_alge(klangc_expr_t *expr);

/**
 * Get the reference of an expression.
 * @param expr expression
 * @return reference expression
 */
klangc_expr_ref_t *klangc_expr_get_ref(klangc_expr_t *expr);

/**
 * Get the application expression of an expression.
 * @param expr expression
 * @return application expression
 */
klangc_expr_appl_t *klangc_expr_get_appl(klangc_expr_t *expr);

/**
 * Get the integer value of an expression.
 * @param expr expression
 * @return integer value
 */
int klangc_expr_get_int(klangc_expr_t *expr);

/**
 * Get the string value of an expression.
 * @param expr expression
 * @return string value
 */
const klangc_str_t *klangc_expr_get_str(klangc_expr_t *expr);

/**
 * Get the lambda expression of an expression.
 * @param expr expression
 * @return lambda expression
 */
klangc_expr_lambda_t *klangc_expr_get_lambda(klangc_expr_t *expr);

/**
 * Get the closure expression of an expression.
 * @param expr expression
 * @return closure expression
 */
klangc_expr_closure_t *klangc_expr_get_closure(klangc_expr_t *expr);

/**
 * Get the position of an expression.
 * @param expr expression
 * @return position
 */
klangc_ipos_t klangc_expr_get_ipos(klangc_expr_t *expr);

// -------------------------------
// Parsers.
// -------------------------------
/**
 * Parse an expression.
 * @param input input
 * @param epopt parse options
 * @param pexpr parsed expression
 * @return parse result
 */
klangc_parse_result_t klangc_expr_parse(klangc_input_t *input,
                                        klangc_expr_parse_opt_t epopt,
                                        klangc_expr_t **pexpr);

// -------------------------------
// Printers.
// -------------------------------
/**
 * Print an expression.
 * @param output output
 * @param prec precedence
 * @param expr expression
 */
void klangc_expr_print(klangc_output_t *output, int prec, klangc_expr_t *expr);

// -------------------------------
// Binders.
// -------------------------------
/**
 * Bind an expression.
 * @param env environment
 * @param expr expression
 * @return bind result
 */
klangc_bind_result_t klangc_expr_bind(klangc_expr_env_t *env,
                                      klangc_expr_t *expr);
