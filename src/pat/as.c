#include "as.h"
#include "../malloc.h"
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