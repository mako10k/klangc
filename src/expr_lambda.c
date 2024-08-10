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

struct klangc_expr_lambda {
  klangc_pat_t *kvl_arg;
  klangc_expr_t *kvl_body;
  klangc_expr_lambda_t *kvl_next;
};

klangc_expr_lambda_t *klangc_expr_lambda_new(klangc_pat_t *arg,
                                             klangc_expr_t *body,
                                             klangc_expr_lambda_t *next) {
  assert(arg != NULL);
  assert(body != NULL);
  klangc_expr_lambda_t *lambda = klangc_malloc(sizeof(klangc_expr_lambda_t));
  lambda->kvl_arg = arg;
  lambda->kvl_body = body;
  lambda->kvl_next = next;
  return lambda;
}

klangc_parse_result_t klangc_expr_lambda_parse(klangc_input_t *input,
                                               klangc_expr_lambda_t **plambda) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c;
  if (!klangc_expect(input, '\\', &c)) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  ipos_ss = klangc_skipspaces(input);
  klangc_pat_t *arg;
  switch (klangc_pat_parse(input, KLANGC_PAT_PARSE_NORMAL, &arg)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(kstderr, "expect <pat>: ['\\' ^<pat> '->' <expr>]\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss = klangc_skipspaces(input);
  if (!klangc_expect(input, '-', &c)) {
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(kstderr,
                  "expect '-' but get '%c': ['\\' <pat> ^'->' <expr>]\n", c);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  if (!klangc_expect(input, '>', &c)) {
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(kstderr,
                  "expect '->' but get '-%c': ['\\' <pat> ^'->' "
                  "<expr>]\n",
                  c);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss = klangc_skipspaces(input);
  klangc_expr_t *body;
  switch (klangc_expr_parse(input, KLANGC_EXPR_PARSE_NORMAL, &body)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(kstderr, "expect <expr>: ['\\' <pat> '->' ^<expr>]\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_ipos_t ipos2 = klangc_input_save(input);
  ipos_ss = klangc_skipspaces(input);
  if (!klangc_expect(input, ';', &c)) {
    klangc_input_restore(input, ipos2);
    *plambda = klangc_expr_lambda_new(arg, body, NULL);
    return KLANGC_PARSE_OK;
  }

  klangc_expr_lambda_t *next = NULL;
  switch (klangc_expr_lambda_parse(input, &next)) {
  case KLANGC_PARSE_OK:
    break;

  case KLANGC_PARSE_NOPARSE:
    klangc_input_restore(input, ipos2);
    break;

  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  *plambda = klangc_expr_lambda_new(arg, body, next);
  return KLANGC_PARSE_OK;
}

void klangc_expr_lambda_print(klangc_output_t *output, int prec,
                              klangc_expr_lambda_t *lambda) {
  assert(lambda != NULL);
  if (lambda->kvl_next == NULL) {
    klangc_printf(output, "\\");
    klangc_pat_print(output, KLANGC_PREC_LOWEST, lambda->kvl_arg);
    klangc_printf(output, " -> ");
    klangc_expr_print(output, KLANGC_PREC_LAMBDA + 1, lambda->kvl_body);
    return;
  }
  if (prec > KLANGC_PREC_LAMBDA)
    klangc_printf(output, "(");
  klangc_indent(output, 2);
  while (1) {
    klangc_printf(output, "\n\\");
    klangc_pat_print(output, KLANGC_PREC_LOWEST, lambda->kvl_arg);
    klangc_printf(output, " -> ");
    klangc_expr_print(output, KLANGC_PREC_LAMBDA + 1, lambda->kvl_body);
    lambda = lambda->kvl_next;
    if (lambda == NULL)
      break;
    klangc_printf(output, ";");
  }
  klangc_indent(output, -2);
  if (prec > KLANGC_PREC_LAMBDA)
    klangc_printf(output, "\n)");
}

klangc_pat_t *klangc_expr_lambda_get_arg(klangc_expr_lambda_t *lambda) {
  return lambda->kvl_arg;
}

klangc_expr_t *klangc_expr_lambda_get_body(klangc_expr_lambda_t *lambda) {
  return lambda->kvl_body;
}

klangc_expr_lambda_t *
klangc_expr_lambda_get_next(klangc_expr_lambda_t *lambda) {
  return lambda->kvl_next;
}

klangc_bind_result_t klangc_expr_lambda_bind(klangc_expr_env_t *env,
                                             klangc_expr_lambda_t *lambda) {
  klangc_expr_env_t *env_inner = klangc_expr_env_new(env);
  klangc_expr_ref_target_t *target = klangc_expr_ref_target_new_lambda(lambda);
  klangc_bind_result_t result =
      klangc_pat_bind(env_inner, lambda->kvl_arg, target);
  if (result != KLANGC_BIND_OK)
    return result;
  return klangc_expr_bind(env_inner, lambda->kvl_body);
}

klangc_unbound_result_t
klangc_expr_lambda_check_unbound(klangc_expr_lambda_t *lambda) {
  return klangc_expr_check_unbound(klangc_expr_lambda_get_body(lambda));
}