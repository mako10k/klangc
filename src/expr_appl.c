#include "expr_appl.h"
#include "expr.h"
#include "malloc.h"
#include "output.h"
#include "types.h"
#include <assert.h>

struct klangc_expr_appl {
  klangc_expr_t *kva_func;
  klangc_expr_t *kva_arg;
};

klangc_expr_appl_t *klangc_expr_appl_new(klangc_expr_t *func,
                                         klangc_expr_t *arg) {
  assert(func != NULL);
  assert(arg != NULL);
  klangc_expr_appl_t *ret = klangc_malloc(sizeof(klangc_expr_appl_t));
  ret->kva_func = func;
  ret->kva_arg = arg;
  return ret;
}

klangc_expr_t *klangc_expr_appl_get_func(klangc_expr_appl_t *appl) {
  assert(appl != NULL);
  return appl->kva_func;
}

klangc_expr_t *klangc_expr_appl_get_arg(klangc_expr_appl_t *appl) {
  assert(appl != NULL);
  return appl->kva_arg;
}

klangc_parse_result_t klangc_expr_appl_parse(klangc_input_t *input,
                                             klangc_expr_appl_t **pexpr_appl);

void klangc_expr_appl_print(klangc_output_t *output, int prec,
                            klangc_expr_appl_t *appl);

klangc_bind_result_t klangc_expr_appl_bind(klangc_expr_env_t *env,
                                           klangc_expr_appl_t *appl) {
  klangc_bind_result_t res = klangc_expr_bind(env, appl->kva_func);
  if (res != KLANGC_BIND_OK) {
    return res;
  }
  return klangc_expr_bind(env, appl->kva_arg);
}

klangc_unbound_result_t
klangc_expr_appl_check_unbound(klangc_expr_appl_t *appl) {
  klangc_unbound_result_t res = klangc_expr_check_unbound(appl->kva_func);
  if (res != KLANGC_UNBOUND_OK) {
    return res;
  }
  return klangc_expr_check_unbound(appl->kva_arg);
}

void klangc_expr_appl_print(klangc_output_t *output, int prec,
                            klangc_expr_appl_t *appl) {
  if (prec > KLANGC_PREC_APPL)
    klangc_printf(output, "(");
  klangc_expr_print(output, KLANGC_PREC_APPL, appl->kva_func);
  klangc_printf(output, " ");
  klangc_expr_print(output, KLANGC_PREC_APPL + 1, appl->kva_arg);
  if (prec > KLANGC_PREC_APPL)
    klangc_printf(output, ")");
}