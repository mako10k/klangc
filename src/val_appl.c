#include "val_appl.h"
#include "malloc.h"
#include <assert.h>

struct klangc_value_appl {
  klangc_value_t *func;
  klangc_value_t *arg;
};

klangc_value_appl_t *klangc_value_appl_new(klangc_value_t *func,
                                           klangc_value_t *arg) {
  assert(func != NULL);
  assert(arg != NULL);
  klangc_value_appl_t *value_appl = klangc_malloc(sizeof(klangc_value_appl_t));
  value_appl->func = func;
  value_appl->arg = arg;
  return value_appl;
}
