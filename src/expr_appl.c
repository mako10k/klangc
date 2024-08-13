#include "expr_appl.h"
#include "expr.h"
#include "input.h"
#include "malloc.h"
#include "output.h"
#include "types.h"
#include <assert.h>

// *******************************
// Function application expression.
// *******************************
// -------------------------------
// Structures.
// -------------------------------
/**
 * Function application expression.
 */
struct klangc_expr_appl {
  /** Function expression */
  klangc_expr_t *kva_func;
  /** Number of arguments */
  int kva_argc;
  /** Arguments */
  klangc_expr_t **kva_args;
};

// -------------------------------
// Constructors.
// -------------------------------
klangc_expr_appl_t *klangc_expr_appl_new(klangc_expr_t *func) {
  assert(func != NULL);
  klangc_expr_appl_t *ret = klangc_malloc(sizeof(klangc_expr_appl_t));
  ret->kva_func = func;
  ret->kva_argc = 0;
  ret->kva_args = NULL;
  return ret;
}

void klangc_expr_appl_add_arg(klangc_expr_appl_t *appl, klangc_expr_t *arg) {
  assert(appl != NULL);
  assert(arg != NULL);
  klangc_expr_t **new_args = klangc_realloc(
      appl->kva_args, sizeof(klangc_expr_t *) * (appl->kva_argc + 1));
  new_args[appl->kva_argc] = arg;
  appl->kva_argc++;
  appl->kva_args = new_args;
}

// -------------------------------
// Accessors.
// -------------------------------
klangc_expr_t *klangc_expr_appl_get_func(klangc_expr_appl_t *appl) {
  assert(appl != NULL);
  return appl->kva_func;
}

unsigned int klangc_expr_appl_get_argc(klangc_expr_appl_t *appl) {
  assert(appl != NULL);
  return appl->kva_argc;
}

klangc_expr_t *klangc_expr_appl_get_arg(klangc_expr_appl_t *appl, int index) {
  assert(appl != NULL);
  assert(index >= 0 && index < appl->kva_argc);
  return appl->kva_args[index];
}

// -------------------------------
// Parsers.
// -------------------------------
klangc_parse_result_t klangc_expr_appl_parse(klangc_input_t *input,
                                             klangc_expr_parse_opt_t epopt,
                                             klangc_expr_t *efunc,
                                             klangc_expr_appl_t **pappl) {
  assert(input != NULL);
  assert(efunc != NULL);
  assert(pappl != NULL);
  klangc_expr_appl_t *appl = klangc_expr_appl_new(efunc);
  if (epopt & KLANGC_EXPR_PARSE_NOAPPL) {
    *pappl = appl;
    return KLANGC_PARSE_OK;
  }
  while (1) {
    klangc_expr_t *arg;
    klangc_parse_result_t res;
    res = klangc_expr_parse(input, KLANGC_EXPR_PARSE_NOAPPL, &arg);
    switch (res) {
    case KLANGC_PARSE_OK:
      klangc_expr_appl_add_arg(appl, arg);
      continue;
    case KLANGC_PARSE_NOPARSE:
      *pappl = appl;
      return KLANGC_PARSE_OK;
    case KLANGC_PARSE_ERROR:
      return KLANGC_PARSE_ERROR;
    }
  }
}

// -------------------------------
// Printers.
// -------------------------------
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

// -------------------------------
// Binders.
// -------------------------------
klangc_bind_result_t klangc_expr_appl_bind(klangc_expr_env_t *env,
                                           klangc_expr_appl_t *appl) {
  assert(env != NULL);
  assert(appl != NULL);
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
