#include "pat_ref.h"
#include "expr_env.h"
#include "malloc.h"
#include "ref.h"
#include "types.h"
#include <assert.h>

// *******************************
// Pattern Reference.
// *******************************
// -------------------------------
// Structures.
// -------------------------------
/**
 * Pattern reference.
 */
struct klangc_pat_ref {
  /** Reference */
  const klangc_symbol_t *kpr_sym;
};

// -------------------------------
// Constructors.
// -------------------------------
klangc_pat_ref_t *klangc_pat_ref_new(const klangc_symbol_t *sym) {
  assert(sym != NULL);
  klangc_pat_ref_t *pref = klangc_malloc(sizeof(klangc_pat_ref_t));
  pref->kpr_sym = sym;
  return pref;
}

// -------------------------------
// Accessors.
// -------------------------------
const klangc_symbol_t *klangc_pat_ref_get_symbol(klangc_pat_ref_t *pref) {
  assert(pref != NULL);
  return pref->kpr_sym;
}

// -------------------------------
// Parsers.
// -------------------------------
klangc_parse_result_t klangc_pat_ref_parse(klangc_input_t *input,
                                           klangc_pat_ref_t **pref) {
  assert(input != NULL);
  assert(pref != NULL);
  const klangc_symbol_t *sym = NULL;
  klangc_parse_result_t res = klangc_ref_parse(input, &sym);
  if (res != KLANGC_PARSE_OK)
    return res;
  *pref = klangc_pat_ref_new(sym);
  return KLANGC_PARSE_OK;
}

// -------------------------------
// Printers.
// -------------------------------
void klangc_pat_ref_print(klangc_output_t *output, klangc_pat_ref_t *pref) {
  assert(output != NULL);
  assert(pref != NULL);
  klangc_ref_print(output, pref->kpr_sym);
}

// -------------------------------
// Binders.
// -------------------------------
klangc_bind_result_t klangc_pat_ref_bind(klangc_expr_env_t *env,
                                         klangc_pat_ref_t *pref,
                                         klangc_expr_ref_target_t *target) {
  assert(env != NULL);
  assert(pref != NULL);
  assert(target != NULL);
  const klangc_symbol_t *sym = klangc_pat_ref_get_symbol(pref);
  klangc_expr_env_put_entry(env, sym, target);
  return KLANGC_BIND_OK;
}