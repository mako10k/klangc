#pragma once

#include "klangc.h"

// *******************************
// Pattern.
// *******************************
// -------------------------------
// Types.
// -------------------------------
/**
 * Pattern type.
 */
typedef enum {
  /** Reference */
  KLANGC_PTYPE_REF,
  /** Algebraic */
  KLANGC_PTYPE_ALGE,
  /** As */
  KLANGC_PTYPE_AS,
  /** Integer */
  KLANGC_PTYPE_INT,
  /** String */
  KLANGC_PTYPE_STRING,
} klangc_pat_type_t;

/**
 * Pattern parse options.
 */
typedef enum {
  KLANGC_PAT_PARSE_NORMAL = 0,
  KLANGC_PAT_PARSE_NOARG = 1,
} klangc_pat_parse_opt_t;

/**
 * Pattern ref operation function.
 */
typedef int (*klangc_pat_foreach_ref_func_t)(klangc_pat_ref_t *ref, void *data);

// -------------------------------
// Constructors.
// -------------------------------
/**
 * Make a new pattern reference.
 * @param pref Reference.
 * @param ipos Input position.
 * @return New pattern reference.
 */
klangc_pat_t *klangc_pat_new_ref(klangc_pat_ref_t *pref, klangc_ipos_t ipos);

/**
 * Make a new pattern algebraic.
 * @param palge Algebraic.
 * @param ipos Input position.
 * @return New pattern algebraic.
 */
klangc_pat_t *klangc_pat_new_alge(klangc_pat_alge_t *palge, klangc_ipos_t ipos);

/**
 * Make a new pattern as.
 * @param pref Variable.
 * @param pat_as Pattern.
 * @param ipos Input position.
 * @return New pattern as.
 */
klangc_pat_t *klangc_pat_new_as(klangc_pat_ref_t *pref, klangc_pat_t *pat_as,
                                klangc_ipos_t ipos);

/**
 * Make a new pattern integer.
 * @param intval Integer value.
 * @param ipos Input position.
 * @return New pattern integer.
 */
klangc_pat_t *klangc_pat_new_int(int intval, klangc_ipos_t ipos);

/**
 * Make a new pattern string.
 * @param strval String value.
 * @param ipos Input position.
 * @return New pattern string.
 */
klangc_pat_t *klangc_pat_new_str(const klangc_str_t *strval,
                                 klangc_ipos_t ipos);

// -------------------------------
// Accessors.
// -------------------------------
/**
 * Get the type of a pattern.
 * @param pat Pattern.
 * @return Type.
 */
klangc_pat_type_t klangc_pat_get_type(klangc_pat_t *pat);

/**
 * Get the algebraic of a pattern.
 * @param pat Pattern.
 * @return Algebraic.
 */
klangc_pat_alge_t *klangc_pat_get_alge(klangc_pat_t *pat);

/**
 * Get the reference of a pattern.
 * @param pat Pattern.
 * @return Reference.
 */
klangc_pat_ref_t *klangc_pat_get_ref(klangc_pat_t *pat);

/**
 * Get the as of a pattern.
 * @param pat Pattern.
 * @return As.
 */
klangc_pat_as_t *klangc_pat_get_as(klangc_pat_t *pat);

/**
 * Get the integer value of a pattern.
 * @param pat Pattern.
 * @return Integer value.
 */
int klangc_pat_get_int(klangc_pat_t *pat);

/**
 * Get the string value of a pattern.
 * @param pat Pattern.
 * @return String value.
 */
const klangc_str_t *klangc_pat_get_str(klangc_pat_t *pat);

// -------------------------------
// Parsers.
// -------------------------------
/**
 * Parse a pattern.
 * @param input Input.
 * @param noarg Parse options.
 * @param ppat Pattern.
 * @return Parse result.
 */
klangc_parse_result_t klangc_pat_parse(klangc_input_t *input,
                                       klangc_pat_parse_opt_t ppopts,
                                       klangc_pat_t **ppat);

// -------------------------------
// Printers.
// -------------------------------
/**
 * Print a pattern.
 * @param output Output.
 * @param prec Precedence.
 * @param pat Pattern.
 */
void klangc_pat_print(klangc_output_t *output, int prec, klangc_pat_t *pat);

// -------------------------------
// Binders.
// -------------------------------
/**
 * Bind a pattern.
 * @param env Environment.
 * @param pat Pattern.
 * @param target Target.
 * @return Bind result.
 */
klangc_bind_result_t klangc_pat_bind(klangc_expr_env_t *env, klangc_pat_t *pat,
                                     klangc_expr_ref_target_t *target);

// -------------------------------
// Misc.
// -------------------------------
/**
 * For each reference in a pattern.
 * @param pat Pattern.
 * @param func Bind function.
 * @param data Data.
 * @return 0 if successful, -1 if not.
 */
int klangc_pat_foreach_ref(klangc_pat_t *pat,
                           klangc_pat_foreach_ref_func_t func, void *data);
