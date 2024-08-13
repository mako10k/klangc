#include "expr_ref.h"
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
  klangc_ref_t *ker_ref;
  /** Target */
  klangc_expr_ref_target_t *ker_target;
};

// -------------------------------
// Constructors.
// -------------------------------
klangc_expr_ref_t *klangc_expr_ref_new(klangc_ref_t *ref) {
  assert(ref != NULL);
  klangc_expr_ref_t *eref = klangc_malloc(sizeof(klangc_expr_ref_t));
  eref->ker_ref = ref;
  eref->ker_target = NULL;
  return eref;
}

void klangc_expr_ref_set_target(klangc_expr_ref_t *eref,
                                klangc_expr_ref_target_t *target) {
  assert(eref != NULL);
  assert(target != NULL);
  eref->ker_target = target;
}

// -------------------------------
// Accessors.
// -------------------------------
klangc_ref_t *klangc_expr_ref_get_ref(klangc_expr_ref_t *eref) {
  assert(eref != NULL);
  return eref->ker_ref;
}

klangc_expr_ref_target_t *klangc_expr_ref_get_target(klangc_expr_ref_t *eref) {
  assert(eref != NULL);
  return eref->ker_target;
}

// -------------------------------
// Parsers.
// -------------------------------
klangc_parse_result_t klangc_expr_ref_parse(klangc_input_t *input,
                                            klangc_expr_ref_t **eref) {
  assert(input != NULL);
  assert(eref != NULL);
  klangc_ref_t *ref;
  switch (klangc_ref_parse(input, &ref)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    return KLANGC_PARSE_NOPARSE;
  case KLANGC_PARSE_ERROR:
    return KLANGC_PARSE_ERROR;
  }
  *eref = klangc_expr_ref_new(ref);
  return KLANGC_PARSE_OK;
}

// -------------------------------
// Printers.
// -------------------------------
void klangc_expr_ref_print(klangc_output_t *output, klangc_expr_ref_t *eref) {
  assert(output != NULL);
  assert(eref != NULL);
  klangc_ref_print(output, eref->ker_ref);
}

// -------------------------------
// Binders.
// -------------------------------
klangc_bind_result_t klangc_expr_ref_bind(klangc_expr_env_t *eenv,
                                          klangc_expr_ref_t *eref) {
  klangc_ref_t *ref = klangc_expr_ref_get_ref(eref);
  klangc_expr_ref_target_t *target = klangc_expr_env_get_entry(eenv, ref);
  if (target == NULL) {
    klangc_ipos_t ipos = klangc_ref_get_ipos(ref);
    klangc_printf_ipos(kstderr, ipos, "W: unbound reference ");
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
