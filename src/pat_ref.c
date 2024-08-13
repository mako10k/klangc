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
  klangc_ref_t *kpr_ref;
};

// -------------------------------
// Constructors.
// -------------------------------
klangc_pat_ref_t *klangc_pat_ref_new(klangc_ref_t *ref) {
  assert(ref != NULL);
  klangc_pat_ref_t *pref = klangc_malloc(sizeof(klangc_pat_ref_t));
  pref->kpr_ref = ref;
  return pref;
}

// -------------------------------
// Accessors.
// -------------------------------
klangc_ref_t *klangc_pat_ref_get_ref(klangc_pat_ref_t *pref) {
  assert(pref != NULL);
  return pref->kpr_ref;
}

// -------------------------------
// Parsers.
// -------------------------------
klangc_parse_result_t klangc_pat_ref_parse(klangc_input_t *input,
                                           klangc_pat_ref_t **pref) {
  assert(input != NULL);
  assert(pref != NULL);
  klangc_ref_t *ref = NULL;
  klangc_parse_result_t res = klangc_ref_parse(input, &ref);
  if (res != KLANGC_PARSE_OK)
    return res;
  *pref = klangc_pat_ref_new(ref);
  return KLANGC_PARSE_OK;
}

// -------------------------------
// Printers.
// -------------------------------
void klangc_pat_ref_print(klangc_output_t *output, klangc_pat_ref_t *pref) {
  assert(output != NULL);
  assert(pref != NULL);
  klangc_ref_print(output, pref->kpr_ref);
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
  klangc_ref_t *ref = klangc_pat_ref_get_ref(pref);
  klangc_expr_env_put_entry(env, ref, target);
  return KLANGC_BIND_OK;
}