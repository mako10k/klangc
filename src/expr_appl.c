#include "expr_appl.h"
#include "expr.h"
#include "input.h"
#include "malloc.h"
#include "output.h"
#include "types.h"
#include <assert.h>

struct klangc_expr_appl {
  klangc_expr_t *kva_func;
  int kva_argc;
  klangc_expr_t **kva_args;
};

klangc_expr_appl_t *klangc_expr_appl_new(klangc_expr_t *func) {
  assert(func != NULL);
  klangc_expr_appl_t *ret = klangc_malloc(sizeof(klangc_expr_appl_t));
  ret->kva_func = func;
  ret->kva_argc = 0;
  ret->kva_args = NULL;
  return ret;
}

void klangc_expr_appl_add_args(klangc_expr_appl_t *appl, int argc,
                               klangc_expr_t **arg) {
  assert(appl != NULL);
  assert(appl->kva_argc + argc >= 0);
  assert(arg != NULL);
  int new_argc = appl->kva_argc + argc;
  int old_argc = appl->kva_argc;
  klangc_expr_t **new_args =
      klangc_realloc(appl->kva_args, sizeof(klangc_expr_t *) * new_argc);
  for (int i = 0; i < argc; i++)
    new_args[old_argc + i] = arg[i];
  appl->kva_argc = new_argc;
  appl->kva_args = new_args;
}

klangc_expr_t *klangc_expr_appl_get_func(klangc_expr_appl_t *appl) {
  assert(appl != NULL);
  return appl->kva_func;
}

klangc_expr_t *klangc_expr_appl_get_arg(klangc_expr_appl_t *appl, int index) {
  assert(appl != NULL);
  assert(index >= 0 && index < appl->kva_argc);
  return appl->kva_args[index];
}

klangc_parse_result_t klangc_expr_appl_parse(klangc_input_t *input,
                                             klangc_expr_parse_opt_t epopt,
                                             klangc_expr_t *efunc,
                                             klangc_expr_appl_t **pexpr_appl) {
  assert(input != NULL);
  assert(efunc != NULL);
  assert(pexpr_appl != NULL);
  klangc_expr_appl_t *appl = klangc_expr_appl_new(efunc);
  if (epopt & KLANGC_EXPR_PARSE_NOAPPL) {
    *pexpr_appl = appl;
    return KLANGC_PARSE_OK;
  }
  while (1) {
    klangc_expr_t *arg;
    klangc_parse_result_t res =
        klangc_expr_parse(input, KLANGC_EXPR_PARSE_NOAPPL, &arg);
    switch (res) {
    case KLANGC_PARSE_OK:
      klangc_expr_appl_add_args(appl, 1, &arg);
      continue;
    case KLANGC_PARSE_NOPARSE:
      *pexpr_appl = appl;
      return KLANGC_PARSE_OK;
    case KLANGC_PARSE_ERROR:
      return KLANGC_PARSE_ERROR;
    }
  }
}

void klangc_expr_appl_print(klangc_output_t *output, int prec,
                            klangc_expr_appl_t *appl);

klangc_bind_result_t klangc_expr_appl_bind(klangc_expr_env_t *env,
                                           klangc_expr_appl_t *appl) {
  klangc_bind_result_t res = klangc_expr_bind(env, appl->kva_func);
  if (res != KLANGC_BIND_OK)
    return res;
  for (int i = 0; i < appl->kva_argc; i++) {
    res = klangc_expr_bind(env, appl->kva_args[i]);
    if (res != KLANGC_BIND_OK)
      return res;
  }
  return KLANGC_BIND_OK;
}

klangc_unbound_result_t
klangc_expr_appl_check_unbound(klangc_expr_appl_t *appl) {
  klangc_unbound_result_t res = klangc_expr_check_unbound(appl->kva_func);
  if (res != KLANGC_UNBOUND_OK) {
    return res;
  }
  for (int i = 0; i < appl->kva_argc; i++) {
    res = klangc_expr_check_unbound(appl->kva_args[i]);
    if (res != KLANGC_UNBOUND_OK)
      return res;
  }
  return KLANGC_UNBOUND_OK;
}

void klangc_expr_appl_print(klangc_output_t *output, int prec,
                            klangc_expr_appl_t *appl) {
  assert(output != NULL);
  assert(appl != NULL);
  if (appl->kva_argc == 0) {
    klangc_expr_print(output, prec, appl->kva_func);
    return;
  }
  if (prec > KLANGC_PREC_APPL)
    klangc_printf(output, "(");
  klangc_expr_print(output, KLANGC_PREC_APPL + 1, appl->kva_func);
  for (int i = 0; i < appl->kva_argc; i++) {
    klangc_printf(output, " ");
    klangc_expr_print(output, KLANGC_PREC_APPL + 1, appl->kva_args[i]);
  }
  if (prec > KLANGC_PREC_APPL)
    klangc_printf(output, ")");
}