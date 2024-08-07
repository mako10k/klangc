#include "expr.h"
#include "expr/closure.h"
#include "expr/closure/bind.h"
#include "expr/closure/entry.h"
#include "expr/lambda.h"
#include "expr/ref.h"
#include "input.h"
#include "malloc.h"
#include "output.h"
#include "parse.h"
#include "symbol.h"
#include <assert.h>

struct klangc_expr_appl {
  klangc_expr_t *kva_func;
  klangc_expr_t *kva_arg;
};

struct klangc_expr {
  klangc_expr_type_t ke_type;
  union {
    klangc_symbol_t *ke_symbol;
    klangc_expr_ref_t *ke_ref;
    int ke_intval;
    char *ke_strval;
    klangc_expr_appl_t *ke_appl;
    klangc_lambda_t *ke_lambda;
    klangc_closure_t *ke_closure;
  };
  klangc_ipos_t ke_ipos;
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

klangc_expr_t *klangc_expr_new_lambda(klangc_lambda_t *lambda,
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

klangc_expr_t *klangc_expr_new_closure(klangc_closure_t *closure,
                                       klangc_ipos_t ipos) {
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->ke_type = KLANGC_ETYPE_CLOSURE;
  expr->ke_closure = closure;
  expr->ke_ipos = ipos;
  return expr;
}

klangc_parse_result_t klangc_expr_parse_no_appl(klangc_input_t *input,
                                                klangc_closure_t *upper,
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
    switch (klangc_expr_parse(input, upper, &expr)) {
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

  klangc_closure_t *closure;
  switch (klangc_closure_parse(input, upper, &closure)) {
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

  char *strval;
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

  klangc_lambda_t *lambda;
  switch (klangc_lambda_parse(input, upper, &lambda)) {
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
                                        klangc_closure_t *enclosed_by,
                                        klangc_expr_t **pexpr) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_expr_t *expr;
  switch (klangc_expr_parse_no_appl(input, enclosed_by, &expr)) {
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
    switch (klangc_expr_parse_no_appl(input, enclosed_by, &arg)) {
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
    if (prec > KLANGC_PREC_APPL)
      klangc_printf(output, "(");
    klangc_expr_print(output, KLANGC_PREC_APPL, expr->ke_appl->kva_func);
    klangc_printf(output, " ");
    klangc_expr_print(output, KLANGC_PREC_APPL + 1, expr->ke_appl->kva_arg);
    if (prec > KLANGC_PREC_APPL)
      klangc_printf(output, ")");
    break;
  case KLANGC_ETYPE_LAMBDA:
    klangc_lambda_print(output, expr->ke_lambda);
    break;
  case KLANGC_ETYPE_CLOSURE:
    klangc_closure_print(output, expr->ke_closure);
    break;
  }
}

int klangc_expr_bind_lambda(klangc_closure_t *closure,
                            klangc_lambda_t *lambda) {
  klangc_expr_t *body = klangc_lambda_get_body(lambda);
  klangc_closure_t *inner = klangc_lambda_get_upper(lambda);
  int ret = klangc_expr_bind(inner, body);
  if (ret < 0)
    return -1;
  int cnt_bound = ret;
  ret = klangc_expr_bind(closure, body);
  if (ret < 0)
    return -1;
  return cnt_bound + ret;
}

int klangc_expr_bind(klangc_closure_t *closure, klangc_expr_t *expr) {
  klangc_closure_t *upper;
  int cnt_bound, ret;

  switch (expr->ke_type) {
  case KLANGC_ETYPE_SYMBOL:
    return 0;
  case KLANGC_ETYPE_REF: {
    klangc_bind_t *bind = klangc_expr_ref_get_bind(expr->ke_ref);
    if (bind == NULL) {
      if (klangc_closure_get_bind(
              closure, klangc_expr_ref_get_ref(expr->ke_ref), &bind, &upper)) {
        klangc_expr_ref_set_bind(expr->ke_ref, bind);
        return 1;
      }
    }
    return 0;
  }
  case KLANGC_ETYPE_INT:
  case KLANGC_ETYPE_STRING:
    return 0;

  case KLANGC_ETYPE_APPL:
    ret = klangc_expr_bind(closure, expr->ke_appl->kva_func);
    if (ret < 0)
      return -1;
    cnt_bound = ret;
    ret = klangc_expr_bind(closure, expr->ke_appl->kva_arg);
    if (ret < 0)
      return -1;
    return cnt_bound + ret;

  case KLANGC_ETYPE_LAMBDA:
    return klangc_expr_bind_lambda(closure, expr->ke_lambda);

  case KLANGC_ETYPE_CLOSURE:
    return klangc_closure_bind(expr->ke_closure);
  }
  return 0;
}

int klangc_expr_bind_ent(klangc_closure_t *closure, klangc_closure_ent_t *ent,
                         void *data) {
  if (klangc_closure_ent_islambda(ent)) {
    klangc_lambda_t *lambda = klangc_closure_ent_get_lambda(ent);
    klangc_expr_t *body = klangc_lambda_get_body(lambda);
    klangc_closure_t *inner = klangc_lambda_get_upper(lambda);
    int ret = klangc_expr_bind(inner, body);
    if (ret < 0)
      return -1;
    return klangc_expr_bind(closure, body);
  }
  if (klangc_closure_ent_isbind(ent)) {
    klangc_bind_t *bind = klangc_closure_ent_get_bind(ent);
    klangc_expr_t *expr = klangc_bind_get_expr(bind);
    return klangc_expr_bind(closure, expr);
  }
  return -1;
}

int klangc_expr_check_unbound(klangc_output_t *output,
                              klangc_closure_t *closure, klangc_expr_t *expr) {
  int cnt_unbound, ret;
  switch (expr->ke_type) {
  case KLANGC_ETYPE_SYMBOL:
    return 0;
  case KLANGC_ETYPE_REF:
    if (klangc_expr_ref_get_bind(expr->ke_ref) == NULL) {
      klangc_ipos_print(output, expr->ke_ipos);
      klangc_printf(output, "Unbound variable: ~%s\n",
                    klangc_expr_ref_get_name(expr->ke_ref));
      return 1;
    }
    return 0;

  case KLANGC_ETYPE_INT:
  case KLANGC_ETYPE_STRING:
    return 0;

  case KLANGC_ETYPE_APPL:
    ret = klangc_expr_check_unbound(output, closure, expr->ke_appl->kva_func);
    if (ret < 0)
      return -1;
    cnt_unbound = ret;
    ret = klangc_expr_check_unbound(output, closure, expr->ke_appl->kva_arg);
    if (ret < 0)
      return -1;
    cnt_unbound += ret;
    return cnt_unbound;

  case KLANGC_ETYPE_LAMBDA:
    ret = klangc_lambda_check_unbound(output, expr->ke_lambda);
    if (ret < 0)
      return -1;
    cnt_unbound = ret;
    return cnt_unbound;

  case KLANGC_ETYPE_CLOSURE:
    ret = klangc_closure_check_unbound(output, expr->ke_closure);
    if (ret < 0)
      return -1;
    cnt_unbound = ret;
    return cnt_unbound;
  }
  assert(0);
}

int klangc_expr_check_unbound_for_walk(klangc_closure_t *closure,
                                       klangc_bind_t *bind, void *data) {
  klangc_output_t *output = (klangc_output_t *)data;
  return klangc_expr_check_unbound(output, closure, klangc_bind_get_expr(bind));
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

klangc_expr_t *klangc_expr_appl_get_func(klangc_expr_appl_t *expr) {
  assert(expr != NULL);
  return expr->kva_func;
}

klangc_expr_t *klangc_expr_appl_get_arg(klangc_expr_appl_t *expr) {
  assert(expr != NULL);
  return expr->kva_arg;
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

klangc_lambda_t *klangc_expr_get_lambda(klangc_expr_t *expr) {
  assert(expr != NULL);
  assert(expr->ke_type == KLANGC_ETYPE_LAMBDA);
  return expr->ke_lambda;
}

klangc_closure_t *klangc_expr_get_closure(klangc_expr_t *expr) {
  assert(expr != NULL);
  assert(expr->ke_type == KLANGC_ETYPE_CLOSURE);
  return expr->ke_closure;
}