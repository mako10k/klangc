#include "val_appl.h"
#include "malloc.h"
#include <assert.h>

struct klangc_value_appl {
  klangc_value_t *kva_func;
  unsigned int kva_argc;
  klangc_value_t **kva_args;
};
                    
klangc_value_appl_t *klangc_value_appl_new(klangc_value_t *func) {
  assert(func != NULL);
  klangc_value_appl_t *value_appl = klangc_malloc(sizeof(klangc_value_appl_t));
  value_appl->kva_func = func;
  value_appl->kva_argc = 0;
  value_appl->kva_args = NULL;
  return value_appl;
}

void klangc_value_appl_add_arg(klangc_value_appl_t *appl,
                               klangc_value_t *varg) {
  assert(appl != NULL);
  assert(varg != NULL);
  appl->kva_argc++;
  appl->kva_args =
      klangc_realloc(appl->kva_args, appl->kva_argc * sizeof(klangc_value_t *));
  appl->kva_args[appl->kva_argc - 1] = varg;
}

klangc_value_t *klangc_value_appl_get_func(klangc_value_appl_t *appl) {
  assert(appl != NULL);
  return appl->kva_func;
}

unsigned int klangc_value_appl_get_argc(klangc_value_appl_t *appl) {
  assert(appl != NULL);
  return appl->kva_argc;
}

klangc_value_t *klangc_value_appl_get_arg(klangc_value_appl_t *appl,
                                          unsigned int i) {
  assert(appl != NULL);
  assert(i < appl->kva_argc);
  return appl->kva_args[i];
}
