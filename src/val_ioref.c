#include "val_ioref.h"
#include "malloc.h"
#include "types.h"

struct klangc_value_ioref {
  klangc_inst_ent_t *kvi_ent;
};

klangc_value_ioref_t *klangc_value_ioref_new(klangc_inst_ent_t *ent) {
  klangc_value_ioref_t *ioref = klangc_malloc(sizeof(klangc_value_ioref_t));
  ioref->kvi_ent = ent;
  return ioref;
}