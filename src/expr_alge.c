#include "expr_alge.h"
#include "expr.h"
#include "malloc.h"
#include "output.h"
#include "symbol.h"
#include "types.h"
#include <assert.h>

// *******************************
// Algebraic expression.
// *******************************

// -------------------------------
// Structures.
// -------------------------------
/**
 * Algebraic expression.
 */
struct klangc_expr_alge {
  /** Constructor symbol */
  klangc_symbol_t *ker_constr;
  /** Number of arguments */
  int ker_argc;
  /** Arguments */
  klangc_expr_t **ker_args;
};

// -------------------------------
// Constructors.
// -------------------------------
klangc_expr_alge_t *klangc_expr_alge_new(klangc_symbol_t *constr) {
  assert(constr != NULL);
  klangc_expr_alge_t *expr = klangc_malloc(sizeof(klangc_expr_alge_t));
  expr->ker_constr = constr;
  expr->ker_argc = 0;
  expr->ker_args = NULL;
  return expr;
}

void klangc_expr_alge_add_arg(klangc_expr_alge_t *alge, klangc_expr_t *arg) {
  assert(alge != NULL);
  assert(arg != NULL);
  klangc_expr_t **new_args = klangc_realloc(
      alge->ker_args, sizeof(klangc_expr_t *) * (alge->ker_argc + 1));
  new_args[alge->ker_argc] = arg;
  alge->ker_argc++;
  alge->ker_args = new_args;
}

// -------------------------------
// Accessors.
// -------------------------------
klangc_symbol_t *klangc_expr_alge_get_constr(klangc_expr_alge_t *expr) {
  assert(expr != NULL);
  return expr->ker_constr;
}

unsigned int klangc_expr_alge_get_argc(klangc_expr_alge_t *expr) {
  assert(expr != NULL);
  return expr->ker_argc;
}

klangc_expr_t *klangc_expr_alge_get_arg(klangc_expr_alge_t *expr, int index) {
  assert(expr != NULL);
  assert(index >= 0 && index < expr->ker_argc);
  return expr->ker_args[index];
}

// -------------------------------
// Parsers.
// -------------------------------
klangc_parse_result_t klangc_expr_alge_parse(klangc_input_t *input,
                                             klangc_expr_parse_opt_t epopt,
                                             klangc_expr_alge_t **pexpr) {
  assert(input != NULL);
  assert(pexpr != NULL);
  klangc_symbol_t *constr = NULL;
  klangc_parse_result_t res = klangc_symbol_parse(input, &constr);
  if (res != KLANGC_PARSE_OK)
    return res;
  klangc_expr_alge_t *alge = klangc_expr_alge_new(constr);
  if (epopt & KLANGC_EXPR_PARSE_NOAPPL) {
    *pexpr = alge;
    return KLANGC_PARSE_OK;
  }
  while (1) {
    klangc_expr_t *arg;
    res = klangc_expr_parse(input, KLANGC_EXPR_PARSE_NOAPPL, &arg);
    switch (res) {
    case KLANGC_PARSE_OK:
      klangc_expr_alge_add_arg(alge, arg);
      break;
    case KLANGC_PARSE_NOPARSE:
      *pexpr = alge;
      return KLANGC_PARSE_OK;
    case KLANGC_PARSE_ERROR:
      return KLANGC_PARSE_ERROR;
    }
  }
}

// -------------------------------
// Printers.
// -------------------------------
void klangc_expr_alge_print(klangc_output_t *output, int prec,
                            klangc_expr_alge_t *expr) {
  assert(output != NULL);
  assert(expr != NULL);
  if (expr->ker_argc == 0) {
    klangc_symbol_print(output, expr->ker_constr);
    return;
  }
  if (prec > KLANGC_PREC_APPL)
    klangc_printf(output, "(");
  klangc_symbol_print(output, expr->ker_constr);
  for (int i = 0; i < expr->ker_argc; i++) {
    klangc_printf(output, " ");
    klangc_expr_print(output, KLANGC_PREC_APPL + 1, expr->ker_args[i]);
  }
  if (prec > KLANGC_PREC_APPL)
    klangc_printf(output, ")");
}

// -------------------------------
// Binders.
// -------------------------------
klangc_bind_result_t klangc_expr_alge_bind(klangc_expr_env_t *upper,
                                           klangc_expr_alge_t *expr) {
  assert(upper != NULL);
  assert(expr != NULL);
  for (int i = 0; i < expr->ker_argc; i++) {
    klangc_expr_t *arg = klangc_expr_alge_get_arg(expr, i);
    klangc_bind_result_t res = klangc_expr_bind(upper, arg);
    if (res != KLANGC_BIND_OK)
      return res;
  }
  return KLANGC_BIND_OK;
}
