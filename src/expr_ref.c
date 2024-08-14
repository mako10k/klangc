#include "expr_ref.h"
#include "expr.h"
#include "expr_env.h"
#include "malloc.h"
#include "output.h"
#include "ref.h"
#include "types.h"
#include <assert.h>

// *******************************
// Reference expression.
// *******************************
// -------------------------------
// Structures.
// -------------------------------
/**
 * Reference expression.
 */
struct klangc_expr_ref {
  /** Reference */
  const klangc_symbol_t *ker_sym;
  /** Target */
  klangc_expr_ref_target_t *ker_target;
  /** Expression */
  klangc_expr_t *ker_expr;
};

// -------------------------------
// Constructors.
// -------------------------------
klangc_expr_ref_t *klangc_expr_ref_new(const klangc_symbol_t *sym) {
  assert(sym != NULL);
  klangc_expr_ref_t *eref = klangc_malloc(sizeof(klangc_expr_ref_t));
  eref->ker_sym = sym;
  eref->ker_target = NULL;
  eref->ker_expr = NULL;
  return eref;
}

void klangc_expr_ref_set_target(klangc_expr_ref_t *eref,
                                klangc_expr_ref_target_t *target) {
  assert(eref != NULL);
  assert(target != NULL);
  assert(eref->ker_target == NULL);
  eref->ker_target = target;
}

void klangc_expr_ref_set_expr(klangc_expr_ref_t *eref, klangc_expr_t *expr) {
  assert(eref != NULL);
  assert(expr != NULL);
  assert(eref->ker_expr == NULL);
  eref->ker_expr = expr;
}

// -------------------------------
// Accessors.
// -------------------------------
const klangc_symbol_t *klangc_expr_ref_get_symbol(klangc_expr_ref_t *eref) {
  assert(eref != NULL);
  return eref->ker_sym;
}

klangc_expr_ref_target_t *klangc_expr_ref_get_target(klangc_expr_ref_t *eref) {
  assert(eref != NULL);
  return eref->ker_target;
}

klangc_expr_t *klangc_expr_ref_get_expr(klangc_expr_ref_t *eref) {
  assert(eref != NULL);
  return eref->ker_expr;
}

// -------------------------------
// Parsers.
// -------------------------------
klangc_parse_result_t klangc_expr_ref_parse(klangc_input_t *input,
                                            klangc_expr_ref_t **eref) {
  assert(input != NULL);
  assert(eref != NULL);
  const klangc_symbol_t *sym;
  switch (klangc_ref_parse(input, &sym)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    return KLANGC_PARSE_NOPARSE;
  case KLANGC_PARSE_ERROR:
    return KLANGC_PARSE_ERROR;
  }
  *eref = klangc_expr_ref_new(sym);
  return KLANGC_PARSE_OK;
}

// -------------------------------
// Printers.
// -------------------------------
void klangc_expr_ref_print(klangc_output_t *output, klangc_expr_ref_t *eref) {
  assert(output != NULL);
  assert(eref != NULL);
  klangc_ref_print(output, eref->ker_sym);
}

// -------------------------------
// Binders.
// -------------------------------
klangc_bind_result_t klangc_expr_ref_bind(klangc_expr_env_t *eenv,
                                          klangc_expr_ref_t *eref) {
  const klangc_symbol_t *sym = klangc_expr_ref_get_symbol(eref);
  klangc_expr_ref_target_t *target = klangc_expr_env_get_entry(eenv, sym);
  if (target == NULL) {
    klangc_expr_t *expr = klangc_expr_ref_get_expr(eref);
    klangc_ipos_t ipos = klangc_expr_get_ipos(expr);
    klangc_printf_ipos(kstderr, ipos, "W: unbound reference ");
    klangc_ref_print(kstderr, sym);
    klangc_printf(kstderr, "\n");
    return KLANGC_BIND_OK;
  }
  if (eref->ker_target != NULL && eref->ker_target != target) {
    klangc_expr_t *expr = klangc_expr_ref_get_expr(eref);
    klangc_ipos_t ipos = klangc_expr_get_ipos(expr);
    klangc_printf_ipos(kstderr, ipos, "W: re-binding reference ");
    klangc_ref_print(kstderr, sym);
    klangc_printf(kstderr, "\n");
    return KLANGC_BIND_ERROR;
  }
  eref->ker_target = target;
  return KLANGC_BIND_OK;
}
