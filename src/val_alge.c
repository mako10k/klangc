#include "val_alge.h"
#include "malloc.h"
#include <assert.h>

struct klangc_value_alge {
  const klangc_symbol_t *kva_constr;
  int kva_argc;
  klangc_value_t **kva_args;
};

klangc_value_alge_t *klangc_value_alge_new(const klangc_symbol_t *constr) {
  klangc_value_alge_t *valge = klangc_malloc(sizeof(klangc_value_alge_t));
  valge->kva_constr = constr;
  valge->kva_argc = 0;
  valge->kva_args = NULL;
  return valge;
}

const klangc_symbol_t *
klangc_value_alge_get_constr(klangc_value_alge_t *valge) {
  assert(valge != NULL);
  return valge->kva_constr;
}

int klangc_value_alge_get_argc(klangc_value_alge_t *valge) {
  assert(valge != NULL);
  return valge->kva_argc;
}

klangc_value_t *klangc_value_alge_get_arg(klangc_value_alge_t *valge,
                                          int index) {
  assert(valge != NULL);
  assert(index >= 0 && index < valge->kva_argc);
  return valge->kva_args[index];
}