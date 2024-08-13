#include "expr_cfunc.h"
#include "malloc.h"
#include "types.h"

struct klangc_expr_cfunc {
  klangc_expr_cfunc_def_t *kec_def;
};

klangc_expr_cfunc_t *klangc_expr_cfunc_new(klangc_expr_cfunc_def_t *def) {
  klangc_expr_cfunc_t *cfunc = klangc_malloc(sizeof(klangc_expr_cfunc_t));
  cfunc->kec_def = def;
  return cfunc;
}