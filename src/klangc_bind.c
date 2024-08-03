#include "klangc_bind.h"
#include "klangc_expr.h"

struct klangc_bind {
  klangc_pattern_t *pat;
  klangc_expr_t *expr;
  klangc_bind_t *next;
  klangc_ipos_t ipos;
};

klangc_bind_t *klangc_bind_new(klangc_pattern_t *pat, klangc_expr_t *expr,
                               klangc_ipos_t ipos) {
  klangc_bind_t *bind = klangc_malloc(sizeof(klangc_bind_t));
  bind->pat = pat;
  bind->expr = expr;
  bind->next = NULL;
  bind->ipos = ipos;
  return bind;
}

klangc_pattern_t *klangc_bind_get_pat(klangc_bind_t *bind) { return bind->pat; }

klangc_expr_t *klangc_bind_get_expr(klangc_bind_t *bind) { return bind->expr; }