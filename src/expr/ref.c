#include "ref.h"
#include "../malloc.h"
#include "../ref.h"
#include <assert.h>

struct klangc_expr_ref {
  klangc_ref_t *ker_ref;
  klangc_expr_closure_bind_t *ker_bind;
};

klangc_expr_ref_t *klangc_expr_ref_new(klangc_ref_t *ref) {
  assert(ref != NULL);
  klangc_expr_ref_t *expr = klangc_malloc(sizeof(klangc_expr_ref_t));
  expr->ker_ref = ref;
  expr->ker_bind = NULL;
  return expr;
}

klangc_ref_t *klangc_expr_ref_get_ref(klangc_expr_ref_t *expr) {
  return expr->ker_ref;
}

klangc_symbol_t *klangc_expr_ref_get_symbol(klangc_expr_ref_t *expr) {
  return klangc_ref_get_symbol(expr->ker_ref);
}

const char *klangc_expr_ref_get_name(klangc_expr_ref_t *expr) {
  return klangc_ref_get_name(expr->ker_ref);
}

klangc_expr_closure_bind_t *klangc_expr_ref_get_bind(klangc_expr_ref_t *expr) {
  return expr->ker_bind;
}

klangc_ipos_t klangc_expr_ref_get_ipos(klangc_expr_ref_t *expr) {
  return klangc_ref_get_ipos(expr->ker_ref);
}

void klangc_expr_ref_set_bind(klangc_expr_ref_t *expr,
                              klangc_expr_closure_bind_t *bind) {
  expr->ker_bind = bind;
}

klangc_parse_result_t klangc_expr_ref_parse(klangc_input_t *input,
                                            klangc_expr_ref_t **pexpr_ref) {
  klangc_ref_t *ref;
  switch (klangc_ref_parse(input, &ref)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    return KLANGC_PARSE_NOPARSE;
  case KLANGC_PARSE_ERROR:
    return KLANGC_PARSE_ERROR;
  }
  klangc_expr_ref_t *expr = klangc_expr_ref_new(ref);
  *pexpr_ref = expr;
  return KLANGC_PARSE_OK;
}

void klangc_expr_ref_print(klangc_output_t *output,
                           klangc_expr_ref_t *expr_ref) {
  klangc_ref_print(output, expr_ref->ker_ref);
}