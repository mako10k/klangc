#include "pat_as.h"
#include "malloc.h"
#include "output.h"
#include "pat.h"
#include "pat_ref.h"
#include <assert.h>

// *******************************
// Pattern As.
// *******************************
// -------------------------------
// Structures.
// -------------------------------
/**
 * Pattern as.
 */
struct klangc_pat_as {
  /** Reference */
  klangc_pat_ref_t *kpas_ref;
  /** Pattern */
  klangc_pat_t *kpas_pat;
};

// -------------------------------
// Constructors.
// -------------------------------
klangc_pat_as_t *klangc_pat_as_new(klangc_pat_ref_t *pref, klangc_pat_t *pat) {
  assert(pref != NULL);
  assert(pat != NULL);
  klangc_pat_as_t *as = klangc_malloc(sizeof(klangc_pat_as_t));
  as->kpas_ref = pref;
  as->kpas_pat = pat;
  return as;
}

// -------------------------------
// Accessors.
// -------------------------------
klangc_pat_ref_t *klangc_pat_as_get_ref(klangc_pat_as_t *pas) {
  assert(pas != NULL);
  return pas->kpas_ref;
}

klangc_pat_t *klangc_pat_as_get_pat(klangc_pat_as_t *pas) {
  assert(pas != NULL);
  return pas->kpas_pat;
}

// -------------------------------
// Printers.
// -------------------------------
void klangc_pat_as_print(klangc_output_t *output, klangc_pat_as_t *pas) {
  assert(output != NULL);
  assert(pas != NULL);
  klangc_pat_ref_print(output, pas->kpas_ref);
  klangc_printf(output, "@");
  klangc_pat_print(output, KLANGC_PREC_APPL + 1, klangc_pat_as_get_pat(pas));
}

// -------------------------------
// Binders.
// -------------------------------
klangc_bind_result_t klangc_pat_as_bind(klangc_expr_env_t *env,
                                        klangc_pat_as_t *pas,
                                        klangc_expr_ref_target_t *target) {
  klangc_bind_result_t res = klangc_pat_ref_bind(env, pas->kpas_ref, target);
  if (res != KLANGC_BIND_OK)
    return res;
  return klangc_pat_bind(env, pas->kpas_pat, target);
}
