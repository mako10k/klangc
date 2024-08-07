#include "appl.h"
#include "../malloc.h"
#include <assert.h>

struct klangc_pat_appl {
  klangc_pat_t *kpap_constr;
  klangc_pat_t *kpap_arg;
};

klangc_pat_appl_t *klangc_pat_appl_new(klangc_pat_t *constr,
                                       klangc_pat_t *arg) {
  assert(constr != NULL);
  assert(arg != NULL);

  klangc_pat_appl_t *appl = klangc_malloc(sizeof(klangc_pat_appl_t));
  appl->kpap_constr = constr;
  appl->kpap_arg = arg;
  return appl;
}

klangc_pat_t *klangc_pat_appl_get_constr(klangc_pat_appl_t *appl) {
  assert(appl != NULL);
  return appl->kpap_constr;
}

klangc_pat_t *klangc_pat_appl_get_arg(klangc_pat_appl_t *appl) {
  assert(appl != NULL);
  return appl->kpap_arg;
}
