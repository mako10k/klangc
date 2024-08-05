#include "klangc_bind.h"

struct klangc_bind {
  klangc_pattern_t *kb_pat;
  klangc_expr_t *kb_expr;
  klangc_ipos_t kb_ipos;
};

klangc_bind_t *klangc_bind_new(klangc_pattern_t *pat, klangc_expr_t *expr,
                               klangc_ipos_t ipos) {
  klangc_bind_t *bind = klangc_malloc(sizeof(klangc_bind_t));
  bind->kb_pat = pat;
  bind->kb_expr = expr;
  bind->kb_ipos = ipos;
  return bind;
}

klangc_pattern_t *klangc_bind_get_pat(klangc_bind_t *bind) { return bind->kb_pat; }

klangc_expr_t *klangc_bind_get_expr(klangc_bind_t *bind) { return bind->kb_expr; }

klangc_ipos_t klangc_bind_get_ipos(klangc_bind_t *bind) { return bind->kb_ipos; }