#include "appl.h"
#include "../malloc.h"
#include <assert.h>

struct klangc_expr_appl {
  klangc_expr_t *kva_func;
  klangc_expr_t *kva_arg;
};

klangc_expr_appl_t *klangc_expr_appl_new(klangc_expr_t *func,
                                         klangc_expr_t *arg) {
  assert(func != NULL);
  assert(arg != NULL);
  klangc_expr_appl_t *ret = klangc_malloc(sizeof(klangc_expr_appl_t));
  ret->kva_func = func;
  ret->kva_arg = arg;
  return ret;
}

klangc_expr_t *klangc_expr_appl_get_func(klangc_expr_appl_t *appl) {
  assert(appl != NULL);
  return appl->kva_func;
}

klangc_expr_t *klangc_expr_appl_get_arg(klangc_expr_appl_t *appl) {
  assert(appl != NULL);
  return appl->kva_arg;
}