#include "pat_as.h"
#include "malloc.h"
#include "output.h"
#include "pat.h"
#include "pat_ref.h"
#include <assert.h>

struct klangc_pat_as {
  klangc_pat_ref_t *kpas_ref;
  klangc_pat_t *kpas_pat;
};

klangc_pat_as_t *klangc_pat_as_new(klangc_pat_ref_t *var, klangc_pat_t *pat) {
  assert(var != NULL);
  assert(pat != NULL);

  klangc_pat_as_t *as = klangc_malloc(sizeof(klangc_pat_as_t));
  as->kpas_ref = var;
  as->kpas_pat = pat;
  return as;
}

klangc_pat_ref_t *klangc_pat_as_get_ref(klangc_pat_as_t *as) {
  assert(as != NULL);
  return as->kpas_ref;
}

klangc_pat_t *klangc_pat_as_get_pat(klangc_pat_as_t *as) {
  assert(as != NULL);
  return as->kpas_pat;
}

klangc_bind_result_t klangc_pat_as_bind(klangc_expr_env_t *env,
                                        klangc_pat_as_t *pat_as,
                                        klangc_expr_ref_target_t *target) {
  klangc_bind_result_t res = klangc_pat_ref_bind(env, pat_as->kpas_ref, target);
  if (res != KLANGC_BIND_OK)
    return res;
  return klangc_pat_bind(env, pat_as->kpas_pat, target);
}

void klangc_pat_as_print(klangc_output_t *output, klangc_pat_as_t *pat_as) {
  assert(output != NULL);
  assert(pat_as != NULL);
  klangc_printf(output, "%s@",
                klangc_pat_ref_get_name(klangc_pat_as_get_ref(pat_as)));
  klangc_pat_print(output, KLANGC_PREC_LOWEST, klangc_pat_as_get_pat(pat_as));
}