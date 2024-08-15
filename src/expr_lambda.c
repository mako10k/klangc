#include "expr_lambda.h"
#include "expr.h"
#include "expr_env.h"
#include "input.h"
#include "malloc.h"
#include "output.h"
#include "parse.h"
#include "pat.h"
#include "ref_target.h"
#include "types.h"
#include <assert.h>

// *******************************
// Lambda expression.
// *******************************
// -------------------------------
// Structures.
// -------------------------------
/**
 * Lambda expression.
 */
struct klangc_expr_lambda {
  /** Parameter count */
  unsigned int kel_paramc;
  /** Parameter patterns */
  klangc_pat_t **kel_params;
  /** Body expression */
  klangc_expr_t *kel_body;
  /** Next lambda expression */
  klangc_expr_lambda_t *kel_next;
};

// -------------------------------
// Constructors.
// -------------------------------
klangc_expr_lambda_t *klangc_expr_lambda_new(klangc_pat_t *pat_param,
                                             klangc_expr_t *expr_body,
                                             klangc_expr_lambda_t *elam_next) {
  assert(pat_param != NULL);
  assert(expr_body != NULL);
  klangc_expr_lambda_t *elam_new = klangc_malloc(sizeof(klangc_expr_lambda_t));
  elam_new->kel_paramc = 1;
  elam_new->kel_params = klangc_malloc(sizeof(klangc_pat_t *));
  elam_new->kel_params[0] = pat_param;
  elam_new->kel_body = expr_body;
  elam_new->kel_next = elam_next;
  return elam_new;
}

// -------------------------------
// Accessors.
// -------------------------------
unsigned int klangc_expr_lambda_get_paramc(klangc_expr_lambda_t *elam) {
  assert(elam != NULL);
  return elam->kel_paramc;
}

klangc_pat_t *klangc_expr_lambda_get_param(klangc_expr_lambda_t *elam,
                                           unsigned int index) {
  assert(elam != NULL);
  assert(index >= 0 && index < elam->kel_paramc);
  return elam->kel_params[index];
}

klangc_expr_t *klangc_expr_lambda_get_body(klangc_expr_lambda_t *elam) {
  return elam->kel_body;
}

klangc_expr_lambda_t *klangc_expr_lambda_get_next(klangc_expr_lambda_t *elam) {
  return elam->kel_next;
}

// -------------------------------
// Parsers.
// -------------------------------
klangc_parse_result_t klangc_expr_lambda_parse(klangc_input_t *input,
                                               klangc_expr_lambda_t **pelam) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c;
  klangc_parse_result_t res = klangc_expect(input, '\\', &c);
  if (res != KLANGC_PARSE_OK) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  ipos_ss = klangc_skipspaces(input);
  klangc_pat_t *pat_param;
  res = klangc_pat_parse(input, KLANGC_PAT_PARSE_NOARG, &pat_param);
  switch (res) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_printf_ipos_expects(
        kstderr, ipos_ss, "<pat>", klangc_getc(input),
        "<lambda> ::= '\\' ^<pat>+ '->' <expr> (';' <lambda>)*;\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_expr_lambda_t *elam_new = klangc_malloc(sizeof(klangc_expr_lambda_t));
  elam_new->kel_paramc = 1;
  elam_new->kel_params = klangc_malloc(sizeof(klangc_pat_t *));
  elam_new->kel_params[0] = pat_param;
  elam_new->kel_body = NULL;
  elam_new->kel_next = NULL;

  while (1) {
    res = klangc_pat_parse(input, KLANGC_PAT_PARSE_NOARG, &pat_param);
    switch (res) {
    case KLANGC_PARSE_OK:
      elam_new->kel_paramc++;
      elam_new->kel_params = klangc_realloc(
          elam_new->kel_params, elam_new->kel_paramc * sizeof(klangc_pat_t *));
      elam_new->kel_params[elam_new->kel_paramc - 1] = pat_param;
      continue;
    case KLANGC_PARSE_NOPARSE:
      break;
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    break;
  }

  ipos_ss = klangc_skipspaces(input);
  res = klangc_expect(input, '-', &c);
  if (res != KLANGC_PARSE_OK) {
    klangc_printf_ipos_expects(
        kstderr, ipos_ss, "'->'", c,
        "<lambda> ::= '\\' <pat>+ ^'->' <expr> (';' <lambda>)*;\n");
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  res = klangc_expect(input, '>', &c);
  if (res != KLANGC_PARSE_OK) {
    klangc_printf_ipos(kstderr, ipos_ss,
                       "expect '->' but get '-%c': <lambda> ::= '\\' <pat>+ "
                       "^'->' <expr> (';' <lambda>)*;\n",
                       c);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss = klangc_skipspaces(input);
  klangc_expr_t *expr_body;
  switch (klangc_expr_parse(input, KLANGC_EXPR_PARSE_NORMAL, &expr_body)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_printf_ipos_expects(
        kstderr, ipos_ss, "<expr>", klangc_getc(input),
        "<lambda> ::= '\\' <pat>+ '->' ^<expr> (';' <lambda>)*;\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_ipos_t ipos2 = klangc_input_save(input);
  klangc_skipspaces(input);
  res = klangc_expect(input, ';', &c);
  if (res != KLANGC_PARSE_OK) {
    *pelam = klangc_expr_lambda_new(pat_param, expr_body, NULL);
    return KLANGC_PARSE_OK;
  }

  klangc_expr_lambda_t *elam_next = NULL;
  res = klangc_expr_lambda_parse(input, &elam_next);
  switch (res) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_input_restore(input, ipos2);
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  elam_new->kel_body = expr_body;
  elam_new->kel_next = elam_next;
  *pelam = elam_new;
  return KLANGC_PARSE_OK;
}

// -------------------------------
// Printers.
// -------------------------------
/**
 * Print a single lambda expression.
 * @param output Output stream.
 * @param prec Precedence.
 * @param elam Lambda expression.
 */
static void klangc_expr_lambda_print_single(klangc_output_t *output, int prec,
                                            klangc_expr_lambda_t *elam) {
  assert(output != NULL);
  assert(elam != NULL);
  int prec2 =
      elam->kel_next == NULL ? KLANGC_PREC_LAMBDA : KLANGC_PREC_LAMBDA + 1;
  if (prec > KLANGC_PREC_LAMBDA) {
    klangc_printf(output, "(");
    prec2 = KLANGC_PREC_LOWEST;
  }
  klangc_printf(output, "\\");
  for (unsigned int i = 0; i < elam->kel_paramc; i++) {
    if (i > 0)
      klangc_printf(output, " ");
    klangc_pat_print(output, KLANGC_PREC_APPL + 1, elam->kel_params[i]);
  }
  klangc_printf(output, " -> ");
  klangc_expr_print(output, prec2, elam->kel_body);
  if (prec > KLANGC_PREC_LAMBDA)
    klangc_printf(output, ")");
  return;
}

void klangc_expr_lambda_print(klangc_output_t *output, int prec,
                              klangc_expr_lambda_t *elam) {
  assert(output != NULL);
  assert(elam != NULL);
  if (elam->kel_next == NULL) {
    klangc_expr_lambda_print_single(output, prec, elam);
    return;
  }
  int prec2 = prec;
  if (prec > KLANGC_PREC_LAMBDAS) {
    klangc_printf(output, "(\n");
    prec2 = KLANGC_PREC_LOWEST;
    klangc_indent(output, 2);
  }
  while (1) {
    klangc_expr_lambda_print_single(output, prec2, elam);
    elam = elam->kel_next;
    if (elam == NULL)
      break;
    klangc_printf(output, ";\n");
  }
  if (prec > KLANGC_PREC_LAMBDAS) {
    klangc_indent(output, -2);
    klangc_printf(output, "\n)");
  }
}

// -------------------------------
// Binders.
// -------------------------------
klangc_bind_result_t klangc_expr_lambda_bind(klangc_expr_env_t *env,
                                             klangc_expr_lambda_t *elam) {
  klangc_expr_env_t *env_inner = klangc_expr_env_new(env);
  klangc_expr_ref_target_t *target = klangc_expr_ref_target_new_lambda(elam);
  for (unsigned int i = 0; i < elam->kel_paramc; i++) {
    klangc_bind_result_t res =
        klangc_pat_bind(env_inner, elam->kel_params[i], target);
    if (res != KLANGC_BIND_OK)
      return res;
  }
  klangc_bind_result_t res = klangc_expr_bind(env_inner, elam->kel_body);
  if (res != KLANGC_BIND_OK)
    return res;
  if (elam->kel_next != NULL)
    return klangc_expr_lambda_bind(env, elam->kel_next);
  return KLANGC_BIND_OK;
}
