#include "expr.h"
#include "env.h"
#include "expr_appl.h"
#include "expr_closure.h"
#include "expr_lambda.h"
#include "expr_ref.h"
#include "input.h"
#include "malloc.h"
#include "output.h"
#include "parse.h"
#include "symbol.h"
#include "types.h"
#include <assert.h>

struct klangc_expr {
  klangc_expr_type_t ke_type;
  union {
    klangc_symbol_t *ke_symbol;
    int ke_intval;
    const char *ke_strval;
    klangc_expr_ref_t *ke_ref;
    klangc_expr_appl_t *ke_appl;
    klangc_expr_lambda_t *ke_lambda;
    klangc_expr_closure_t *ke_closure;
  };
  klangc_ipos_t ke_ipos;
};

klangc_expr_t *klangc_expr_new_symbol(klangc_symbol_t *symbol,
                                      klangc_ipos_t ipos) {
  assert(symbol != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->ke_type = KLANGC_ETYPE_SYMBOL;
  expr->ke_symbol = symbol;
  expr->ke_ipos = ipos;
  return expr;
}

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

klangc_parse_result_t klangc_expr_parse_no_appl(klangc_input_t *input,
                                                klangc_expr_t **pexpr) {
  assert(input != NULL);

  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  do {
    int c = klangc_getc(input);
    if (c != '(')
      break;
    ipos_ss = klangc_skipspaces(input);
    klangc_expr_t *expr;
    switch (klangc_expr_parse(input, &expr)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      klangc_ipos_print(kstderr, ipos_ss);
      klangc_printf(kstderr, "expect <expr>: ['(' ^<expr> ')']\n");
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }

    ipos_ss = klangc_skipspaces(input);
    c = klangc_getc(input);
    if (c != ')') {
      klangc_ipos_print(kstderr, ipos_ss);
      klangc_printf(kstderr, "expect ')' but get '%c': ['(' <expr> ^')']\n", c);
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    *pexpr = expr;
    return KLANGC_PARSE_OK;
  } while (0);

  klangc_input_restore(input, ipos_ss);

  klangc_expr_closure_t *closure;
  switch (klangc_expr_closure_parse(input, &closure)) {
  case KLANGC_PARSE_OK:
    *pexpr = klangc_expr_new_closure(closure, ipos_ss);
    return KLANGC_PARSE_OK;
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_symbol_t *symbol;
  switch (klangc_symbol_parse(input, &symbol)) {
  case KLANGC_PARSE_OK:
    *pexpr = klangc_expr_new_symbol(symbol, ipos_ss);
    return KLANGC_PARSE_OK;
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_expr_ref_t *ref;
  switch (klangc_expr_ref_parse(input, &ref)) {
  case KLANGC_PARSE_OK:
    *pexpr = klangc_expr_new_ref(ref, ipos_ss);
    return KLANGC_PARSE_OK;
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  int intval;
  switch (klangc_int_parse(input, &intval)) {
  case KLANGC_PARSE_OK:
    *pexpr = klangc_expr_new_int(intval, ipos_ss);
    return KLANGC_PARSE_OK;
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  const char *strval;
  switch (klangc_string_parse(input, &strval)) {
  case KLANGC_PARSE_OK:
    *pexpr = klangc_expr_new_string(strval, ipos_ss);
    return KLANGC_PARSE_OK;
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_expr_lambda_t *lambda;
  switch (klangc_expr_lambda_parse(input, &lambda)) {
  case KLANGC_PARSE_OK:
    *pexpr = klangc_expr_new_lambda(lambda, ipos_ss);
    return KLANGC_PARSE_OK;
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_input_restore(input, ipos);
  return KLANGC_PARSE_NOPARSE;
}

klangc_parse_result_t klangc_expr_parse(klangc_input_t *input,
                                        klangc_expr_t **pexpr) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_expr_t *expr;
  switch (klangc_expr_parse_no_appl(input, &expr)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(
        kstderr,
        "expect <expr(w/o appl)>: [^<expr(w/o appl)> <expr(w/o appl)>*]\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  while (1) {
    ipos_ss = klangc_skipspaces(input);
    klangc_expr_t *arg;
    switch (klangc_expr_parse_no_appl(input, &arg)) {
    case KLANGC_PARSE_OK:
      break;
    case KLANGC_PARSE_NOPARSE:
      *pexpr = expr;
      return KLANGC_PARSE_OK;
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    klangc_expr_appl_t *appl = klangc_expr_appl_new(expr, arg);
    expr = klangc_expr_new_appl(appl, ipos_ss);
  }
}

void klangc_expr_print(klangc_output_t *output, int prec, klangc_expr_t *expr) {
  switch (expr->ke_type) {
  case KLANGC_ETYPE_SYMBOL:
    klangc_symbol_print(output, expr->ke_symbol);
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
    break;
  case KLANGC_ETYPE_LAMBDA:
    klangc_expr_lambda_print(output, expr->ke_lambda);
    break;
  case KLANGC_ETYPE_CLOSURE:
    klangc_expr_closure_print(output, expr->ke_closure);
    break;
  }
}

klangc_bind_result_t klangc_expr_bind(klangc_expr_env_t *env,
                                      klangc_expr_t *expr) {
  switch (expr->ke_type) {
  case KLANGC_ETYPE_SYMBOL:
  case KLANGC_ETYPE_INT:
  case KLANGC_ETYPE_STRING:
    return KLANGC_BIND_OK;
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
  case KLANGC_ETYPE_SYMBOL:
    return KLANGC_UNBOUND_OK;

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

klangc_symbol_t *klangc_expr_get_symbol(klangc_expr_t *expr) {
  assert(expr != NULL);
  assert(expr->ke_type == KLANGC_ETYPE_SYMBOL);
  return expr->ke_symbol;
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