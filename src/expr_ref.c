#include "expr_ref.h"
#include "expr_env.h"
#include "malloc.h"
#include "output.h"
#include "ref.h"
#include "types.h"
#include <assert.h>

struct klangc_expr_ref {
  klangc_ref_t *ker_ref;
  klangc_expr_ref_target_t *ker_target;
};

klangc_expr_ref_t *klangc_expr_ref_new(klangc_ref_t *ref) {
  assert(ref != NULL);
  klangc_expr_ref_t *expr = klangc_malloc(sizeof(klangc_expr_ref_t));
  expr->ker_ref = ref;
  expr->ker_target = NULL;
  return expr;
}

klangc_ref_t *klangc_expr_ref_get_ref(klangc_expr_ref_t *expr) {
  return expr->ker_ref;
}

klangc_expr_ref_target_t *klangc_expr_ref_get_target(klangc_expr_ref_t *expr) {
  return expr->ker_target;
}

void klangc_expr_ref_set_target(klangc_expr_ref_t *expr,
                                klangc_expr_ref_target_t *target) {
  expr->ker_target = target;
}

klangc_ipos_t klangc_expr_ref_get_ipos(klangc_expr_ref_t *expr) {
  return klangc_ref_get_ipos(expr->ker_ref);
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
  *pexpr_ref = klangc_expr_ref_new(ref);
  return KLANGC_PARSE_OK;
}

void klangc_expr_ref_print(klangc_output_t *output,
                           klangc_expr_ref_t *expr_ref) {
  klangc_ref_print(output, expr_ref->ker_ref);
}

klangc_bind_result_t klangc_expr_ref_bind(klangc_expr_env_t *env,
                                          klangc_expr_ref_t *eref) {
  klangc_ref_t *ref = klangc_expr_ref_get_ref(eref);
  klangc_expr_ref_target_t *target = klangc_expr_env_get_entry(env, ref);
  if (target == NULL) {
    klangc_ipos_t ipos = klangc_ref_get_ipos(ref);
    klangc_printf_ipos(kstderr, ipos, "N: unbound reference ");
    klangc_ref_print(kstderr, ref);
    klangc_printf(kstderr, "\n");
    return KLANGC_BIND_OK;
  }
  if (eref->ker_target != NULL && eref->ker_target != target) {
    klangc_ipos_t ipos = klangc_ref_get_ipos(ref);
    klangc_printf_ipos(kstderr, ipos, "W: re-binding reference ");
    klangc_ref_print(kstderr, ref);
    klangc_printf(kstderr, "\n");
    return KLANGC_BIND_ERROR;
  }
  eref->ker_target = target;
  return KLANGC_BIND_OK;
}

klangc_unbound_result_t klangc_expr_ref_check_unbound(klangc_expr_ref_t *eref) {
  if (eref->ker_target == NULL) {
    klangc_ref_t *ref = klangc_expr_ref_get_ref(eref);
    klangc_ipos_t ipos = klangc_ref_get_ipos(ref);
    klangc_printf_ipos(kstderr, ipos, "E: unbound reference ");
    klangc_ref_print(kstderr, ref);
    klangc_printf(kstderr, "\n");
    return KLANGC_UNBOUND_ERROR;
  }
  return KLANGC_UNBOUND_OK;
}