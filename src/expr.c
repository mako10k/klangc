#include "expr.h"
#include "env.h"
#include "expr_alge.h"
#include "expr_appl.h"
#include "expr_closure.h"
#include "expr_lambda.h"
#include "expr_ref.h"
#include "input.h"
#include "malloc.h"
#include "output.h"
#include "parse.h"
#include "types.h"
#include <assert.h>

struct klangc_expr {
  klangc_expr_type_t ke_type;
  union {
    int ke_intval;
    const char *ke_strval;
    klangc_expr_ref_t *ke_ref;
    klangc_expr_appl_t *ke_appl;
    klangc_expr_alge_t *ke_alge;
    klangc_expr_lambda_t *ke_lambda;
    klangc_expr_closure_t *ke_closure;
  };
  klangc_ipos_t ke_ipos;
};

klangc_expr_t *klangc_expr_new_ref(klangc_expr_ref_t *ref, klangc_ipos_t ipos) {
  assert(ref != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->ke_type = KLANGC_ETYPE_REF;
  expr->ke_ref = ref;
  expr->ke_ipos = ipos;
  return expr;
}

klangc_expr_t *klangc_expr_new_int(int intval, klangc_ipos_t ipos) {
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->ke_type = KLANGC_ETYPE_INT;
  expr->ke_intval = intval;
  expr->ke_ipos = ipos;
  return expr;
}

klangc_expr_t *klangc_expr_new_string(const char *strval, klangc_ipos_t ipos) {
  assert(strval != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->ke_type = KLANGC_ETYPE_STRING;
  expr->ke_strval = klangc_strdup(strval);
  expr->ke_ipos = ipos;
  return expr;
}

klangc_expr_t *klangc_expr_new_lambda(klangc_expr_lambda_t *lambda,
                                      klangc_ipos_t ipos) {
  assert(lambda != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->ke_type = KLANGC_ETYPE_LAMBDA;
  expr->ke_lambda = lambda;
  expr->ke_ipos = ipos;
  return expr;
}

klangc_expr_t *klangc_expr_new_alge(klangc_expr_alge_t *alge,
                                    klangc_ipos_t ipos) {
  assert(alge != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->ke_type = KLANGC_ETYPE_ALGE;
  expr->ke_alge = alge;
  expr->ke_ipos = ipos;
  return expr;
}

klangc_expr_t *klangc_expr_new_appl(klangc_expr_appl_t *appl,
                                    klangc_ipos_t ipos) {
  assert(appl != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->ke_type = KLANGC_ETYPE_APPL;
  expr->ke_appl = appl;
  expr->ke_ipos = ipos;
  return expr;
}

klangc_expr_t *klangc_expr_new_closure(klangc_expr_closure_t *closure,
                                       klangc_ipos_t ipos) {
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->ke_type = KLANGC_ETYPE_CLOSURE;
  expr->ke_closure = closure;
  expr->ke_ipos = ipos;
  return expr;
}

static klangc_parse_result_t klangc_expr_parse_paren(klangc_input_t *input,
                                                     klangc_expr_t **pexpr) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  if (!klangc_expect(input, '(', NULL)) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  ipos_ss = klangc_skipspaces(input);
  klangc_expr_t *expr;
  switch (klangc_expr_parse(input, KLANGC_EXPR_PARSE_NORMAL, &expr)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_printf_ipos_expects(kstderr, ipos_ss, "<expr>", klangc_getc(input),
                               "<expr> ::= .. | '(' ^<expr> ')' | ..;\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss = klangc_skipspaces(input);
  int c;
  if (!klangc_expect(input, ')', &c)) {
    klangc_printf_ipos_expects(kstderr, ipos_ss, "'('", c,
                               "<expr> ::= .. | '(' <expr> ^')' | ..;\n");
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  *pexpr = expr;
  return KLANGC_PARSE_OK;
}

klangc_parse_result_t klangc_expr_parse_closure(klangc_input_t *input,
                                                klangc_expr_t **pexpr) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_expr_closure_t *closure;
  klangc_parse_result_t res;
  res = klangc_expr_closure_parse(input, &closure);
  switch (res) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return res;
  }
  *pexpr = klangc_expr_new_closure(closure, ipos_ss);
  return KLANGC_PARSE_OK;
}

klangc_parse_result_t klangc_expr_parse_alge(klangc_input_t *input,
                                             klangc_expr_parse_opt_t epopt,
                                             klangc_expr_t **pexpr) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_expr_alge_t *alge;
  klangc_parse_result_t res;
  res = klangc_expr_alge_parse(input, epopt, &alge);
  if (res != KLANGC_PARSE_OK) {
    klangc_input_restore(input, ipos);
    return res;
  }
  *pexpr = klangc_expr_new_alge(alge, ipos_ss);
  return KLANGC_PARSE_OK;
}

klangc_parse_result_t klangc_expr_parse_appl(klangc_input_t *input,
                                             klangc_expr_parse_opt_t epopt,
                                             klangc_expr_t *efunc,
                                             klangc_expr_t **pexpr) {
  klangc_expr_appl_t *appl;
  klangc_parse_result_t res;
  res = klangc_expr_appl_parse(input, epopt, efunc, &appl);
  if (res != KLANGC_PARSE_OK)
    return res;
  *pexpr = klangc_expr_new_appl(appl, klangc_expr_get_ipos(efunc));
  return KLANGC_PARSE_OK;
}

klangc_parse_result_t klangc_expr_parse_ref(klangc_input_t *input,
                                            klangc_expr_t **pexpr) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_expr_ref_t *ref;
  klangc_parse_result_t res;
  res = klangc_expr_ref_parse(input, &ref);
  if (res != KLANGC_PARSE_OK) {
    klangc_input_restore(input, ipos);
    return res;
  }
  *pexpr = klangc_expr_new_ref(ref, ipos_ss);
  return KLANGC_PARSE_OK;
}

klangc_parse_result_t klangc_expr_parse_int(klangc_input_t *input,
                                            klangc_expr_t **pexpr) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int intval;
  klangc_parse_result_t res = klangc_int_parse(input, &intval);
  if (res != KLANGC_PARSE_OK) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  *pexpr = klangc_expr_new_int(intval, ipos_ss);
  return KLANGC_PARSE_OK;
}

klangc_parse_result_t klangc_expr_parse_string(klangc_input_t *input,
                                               klangc_expr_t **pexpr) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  const char *strval;
  klangc_parse_result_t res = klangc_string_parse(input, &strval);
  if (res != KLANGC_PARSE_OK) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  *pexpr = klangc_expr_new_string(strval, ipos_ss);
  return KLANGC_PARSE_OK;
}

klangc_parse_result_t klangc_expr_parse_lambda(klangc_input_t *input,
                                               klangc_expr_t **pexpr) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_expr_lambda_t *lambda;
  klangc_parse_result_t res;
  res = klangc_expr_lambda_parse(input, &lambda);
  if (res != KLANGC_PARSE_OK) {
    klangc_input_restore(input, ipos);
    return res;
  }
  *pexpr = klangc_expr_new_lambda(lambda, ipos_ss);
  return KLANGC_PARSE_OK;
}

klangc_parse_result_t klangc_expr_parse_noappl(klangc_input_t *input,
                                               klangc_expr_parse_opt_t epopt,
                                               klangc_expr_t **pexpr) {
  assert(input != NULL);
  klangc_parse_result_t res;
  klangc_expr_t *expr;
  res = klangc_expr_parse_paren(input, pexpr);
  if (res != KLANGC_PARSE_NOPARSE)
    return res;

  res = klangc_expr_parse_closure(input, &expr);
  if (res != KLANGC_PARSE_NOPARSE)
    return res;

  res = klangc_expr_parse_alge(input, epopt, &expr);
  if (res != KLANGC_PARSE_NOPARSE)
    return res;

  res = klangc_expr_parse_ref(input, &expr);
  if (res != KLANGC_PARSE_NOPARSE)
    return res;

  res = klangc_expr_parse_int(input, &expr);
  if (res != KLANGC_PARSE_NOPARSE)
    return res;

  res = klangc_expr_parse_string(input, &expr);
  if (res != KLANGC_PARSE_NOPARSE)
    return res;

  res = klangc_expr_parse_lambda(input, &expr);
  if (res != KLANGC_PARSE_NOPARSE)
    return res;

  return KLANGC_PARSE_NOPARSE;
}

klangc_parse_result_t klangc_expr_parse(klangc_input_t *input,
                                        klangc_expr_parse_opt_t epopt,
                                        klangc_expr_t **pexpr) {
  assert(input != NULL);
  assert(pexpr != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_parse_result_t res;
  klangc_expr_t *expr;
  res = klangc_expr_parse_noappl(input, epopt, &expr);
  if (res != KLANGC_PARSE_OK)
    return res;
  if (epopt & KLANGC_EXPR_PARSE_NOAPPL) {
    klangc_expr_t *func = expr;
    res = klangc_expr_parse_appl(input, epopt, func, &expr);
    if (res == KLANGC_PARSE_ERROR) {
      klangc_input_restore(input, ipos);
      return res;
    }
  }
  *pexpr = expr;
  return KLANGC_PARSE_OK;
}

void klangc_expr_print(klangc_output_t *output, int prec, klangc_expr_t *expr) {
  switch (expr->ke_type) {
  case KLANGC_ETYPE_ALGE:
    klangc_expr_alge_print(output, prec, expr->ke_alge);
    break;
  case KLANGC_ETYPE_REF:
    klangc_expr_ref_print(output, expr->ke_ref);
    break;
  case KLANGC_ETYPE_INT:
    klangc_printf(output, "%d", expr->ke_intval);
    break;
  case KLANGC_ETYPE_STRING:
    klangc_printf(output, "\"%s\"", expr->ke_strval);
    break;
  case KLANGC_ETYPE_APPL:
    klangc_expr_appl_print(output, prec, expr->ke_appl);
    break;
  case KLANGC_ETYPE_LAMBDA:
    klangc_expr_lambda_print(output, prec, expr->ke_lambda);
    break;
  case KLANGC_ETYPE_CLOSURE:
    klangc_expr_closure_print(output, expr->ke_closure);
    break;
  }
}

klangc_bind_result_t klangc_expr_bind(klangc_expr_env_t *env,
                                      klangc_expr_t *expr) {
  switch (expr->ke_type) {
  case KLANGC_ETYPE_INT:
  case KLANGC_ETYPE_STRING:
    return KLANGC_BIND_OK;
  case KLANGC_ETYPE_ALGE:
    return klangc_expr_alge_bind(env, expr->ke_alge);
  case KLANGC_ETYPE_REF:
    return klangc_expr_ref_bind(env, expr->ke_ref);

  case KLANGC_ETYPE_APPL:
    return klangc_expr_appl_bind(env, expr->ke_appl);

  case KLANGC_ETYPE_LAMBDA:
    return klangc_expr_lambda_bind(env, expr->ke_lambda);

  case KLANGC_ETYPE_CLOSURE:
    return klangc_expr_closure_bind(env, expr->ke_closure);
  }
  klangc_printf(kstderr, "Unknown expression type: %d\n", expr->ke_type);
  return KLANGC_BIND_ERROR;
}

klangc_unbound_result_t klangc_expr_check_unbound(klangc_expr_t *expr) {
  switch (expr->ke_type) {
  case KLANGC_ETYPE_ALGE:
    return klangc_expr_alge_check_unbound(expr->ke_alge);

  case KLANGC_ETYPE_REF:
    return klangc_expr_ref_check_unbound(expr->ke_ref);

  case KLANGC_ETYPE_INT:
  case KLANGC_ETYPE_STRING:
    return KLANGC_UNBOUND_OK;

  case KLANGC_ETYPE_APPL:
    return klangc_expr_appl_check_unbound(expr->ke_appl);

  case KLANGC_ETYPE_LAMBDA:
    return klangc_expr_lambda_check_unbound(expr->ke_lambda);

  case KLANGC_ETYPE_CLOSURE:
    return klangc_expr_closure_check_unbound(expr->ke_closure);
  }
  klangc_printf(kstderr, "Unknown expression type: %d\n", expr->ke_type);
  return KLANGC_UNBOUND_ERROR;
}

klangc_ipos_t klangc_expr_get_ipos(klangc_expr_t *expr) {
  assert(expr != NULL);
  return expr->ke_ipos;
}

klangc_expr_type_t klangc_expr_get_type(klangc_expr_t *expr) {
  assert(expr != NULL);
  return expr->ke_type;
}

klangc_expr_ref_t *klangc_expr_get_ref(klangc_expr_t *expr) {
  assert(expr != NULL);
  assert(expr->ke_type == KLANGC_ETYPE_REF);
  return expr->ke_ref;
}

klangc_expr_appl_t *klangc_expr_get_appl(klangc_expr_t *expr) {
  assert(expr != NULL);
  assert(expr->ke_type == KLANGC_ETYPE_APPL);
  return expr->ke_appl;
}

int klangc_expr_get_int(klangc_expr_t *expr) {
  assert(expr != NULL);
  assert(expr->ke_type == KLANGC_ETYPE_INT);
  return expr->ke_intval;
}

const char *klangc_expr_get_string(klangc_expr_t *expr) {
  assert(expr != NULL);
  assert(expr->ke_type == KLANGC_ETYPE_STRING);
  return expr->ke_strval;
}

klangc_expr_lambda_t *klangc_expr_get_lambda(klangc_expr_t *expr) {
  assert(expr != NULL);
  assert(expr->ke_type == KLANGC_ETYPE_LAMBDA);
  return expr->ke_lambda;
}

klangc_expr_closure_t *klangc_expr_get_closure(klangc_expr_t *expr) {
  assert(expr != NULL);
  assert(expr->ke_type == KLANGC_ETYPE_CLOSURE);
  return expr->ke_closure;
}