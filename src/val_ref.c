#include "val_ref.h"
#include "malloc.h"
#include <assert.h>

struct klangc_value_ref {
  klangc_ref_t *ref;
};

klangc_value_ref_t *klangc_value_ref_from_expr_ref(klangc_ref_t *ref) {
  assert(ref != NULL);
  klangc_value_ref_t *value_ref = klangc_malloc(sizeof(klangc_value_ref_t));
  value_ref->ref = ref;
  return value_ref;
}
