#include "klangc_expr.h"
#include "klangc_bare_closure.h"
#include "klangc_bind.h"
#include "klangc_eclosure.h"
#include "klangc_input.h"
#include "klangc_lambda.h"
#include "klangc_malloc.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_pattern.h"
#include "klangc_ref.h"
#include "klangc_symbol.h"
#include "klangc_types.h"

typedef enum klangc_expr_type {
  KLANGC_ETYPE_SYMBOL,
  KLANGC_ETYPE_REF,
  KLANGC_ETYPE_APPL,
  KLANGC_ETYPE_INT,
  KLANGC_ETYPE_STRING,
  KLANGC_ETYPE_LAMBDA,
  KLANGC_ETYPE_CLOSURE,
} klangc_expr_type_t;

struct klangc_expr_appl {
  klangc_expr_t *kva_func;
  klangc_expr_t *kva_arg;
};

struct klangc_expr {
  klangc_expr_type_t type;
  union {
    klangc_symbol_t *symbol;
    struct {
      klangc_ref_t *ref;
      klangc_bind_t *ref_ref;
    };
    int intval;
    char *strval;
    klangc_expr_appl_t *kv_appl;
    klangc_lambda_t *kv_lambda;
    klangc_bare_closure_t *kv_closure;
  };
  klangc_ipos_t ipos;
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
  expr->type = KLANGC_ETYPE_SYMBOL;
  expr->symbol = symbol;
  expr->ipos = ipos;
  return expr;
}

klangc_expr_t *klangc_expr_new_ref(klangc_ref_t *ref, klangc_ipos_t ipos) {
  assert(ref != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_REF;
  expr->ref = ref;
  expr->ipos = ipos;
  return expr;
}

klangc_expr_t *klangc_expr_new_int(int intval, klangc_ipos_t ipos) {
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_INT;
  expr->intval = intval;
  expr->ipos = ipos;
  return expr;
}

klangc_expr_t *klangc_expr_new_string(const char *strval, klangc_ipos_t ipos) {
  assert(strval != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_STRING;
  expr->strval = klangc_strdup(strval);
  expr->ipos = ipos;
  return expr;
}

klangc_expr_t *klangc_expr_new_lambda(klangc_lambda_t *lambda,
                                      klangc_ipos_t ipos) {
  assert(lambda != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_LAMBDA;
  expr->kv_lambda = lambda;
  expr->ipos = ipos;
  return expr;
}

klangc_expr_t *klangc_expr_new_appl(klangc_expr_appl_t *appl,
                                    klangc_ipos_t ipos) {
  assert(appl != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_APPL;
  expr->kv_appl = appl;
  expr->ipos = ipos;
  return expr;
}

klangc_expr_t *klangc_expr_new_closure(klangc_bare_closure_t *closure,
                                       klangc_ipos_t ipos) {
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_CLOSURE;
  expr->kv_closure = closure;
  expr->ipos = ipos;
  return expr;
}

klangc_parse_result_t klangc_expr_lambda_parse(klangc_input_t *input,
                                               klangc_bare_closure_t *upper,
                                               klangc_lambda_t **plambda) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c;
  if (!klangc_expect(input, '\\', &c)) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  ipos_ss = klangc_skipspaces(input);
  klangc_pattern_t *arg;
  switch (klangc_pattern_parse(input, &arg)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(kstderr, "expect <pattern>: ['\\' ^<pattern> '->' <expr>]\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss = klangc_skipspaces(input);
  if (!klangc_expect(input, '-', &c)) {
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(
        kstderr, "expect '-' but get '%c': ['\\' <pattern> ^'->' <expr>]\n", c);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  if (!klangc_expect(input, '>', &c)) {
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(kstderr,
                  "expect '->' but get '-%c': ['\\' <pattern> ^'->' "
                  "<expr>]\n",
                  c);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss = klangc_skipspaces(input);
  klangc_expr_t *body;
  switch (klangc_expr_parse(input, upper, &body)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_ipos_print(kstderr, ipos_ss);
    klangc_printf(kstderr, "expect <expr>: ['\\' <pattern> '->' ^<expr>]\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  *plambda = klangc_expr_lambda_new(arg, body, upper, ipos_ss);
  return KLANGC_PARSE_OK;
}

klangc_parse_result_t
klangc_expr_parse_no_appl(klangc_input_t *input,
                          klangc_bare_closure_t *enclosed_by,
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
    switch (klangc_expr_parse(input, enclosed_by, &expr)) {
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

  klangc_bare_closure_t *closure;
  switch (klangc_expr_closure_parse(input, enclosed_by, &closure)) {
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

  klangc_ref_t *ref;
  switch (klangc_ref_parse(input, &ref)) {
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
  switch (klangc_expr_lambda_parse(input, enclosed_by, &lambda)) {
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
                                        klangc_bare_closure_t *enclosed_by,
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
  switch (expr->type) {
  case KLANGC_ETYPE_SYMBOL:
    klangc_symbol_print(output, expr->symbol);
    break;
  case KLANGC_ETYPE_REF:
    klangc_ref_print(output, expr->ref);
    break;
  case KLANGC_ETYPE_INT:
    klangc_printf(output, "%d", expr->intval);
    break;
  case KLANGC_ETYPE_STRING:
    klangc_printf(output, "\"%s\"", expr->strval);
    break;
  case KLANGC_ETYPE_APPL:
    if (prec > KLANGC_PREC_APPL)
      klangc_printf(output, "(");
    klangc_expr_print(output, KLANGC_PREC_APPL, expr->kv_appl->kva_func);
    klangc_printf(output, " ");
    klangc_expr_print(output, KLANGC_PREC_APPL + 1, expr->kv_appl->kva_arg);
    if (prec > KLANGC_PREC_APPL)
      klangc_printf(output, ")");
    break;
  case KLANGC_ETYPE_LAMBDA:
    klangc_expr_lambda_print(output, expr->kv_lambda);
    break;
  case KLANGC_ETYPE_CLOSURE:
    klangc_expr_closure_print(output, expr->kv_closure);
    break;
  }
}

int klangc_expr_bind_for_walk(klangc_bare_closure_t *closure,
                              klangc_bind_t *bind, void *data) {
  return klangc_expr_bind(closure, klangc_bind_get_expr(bind));
}

int klangc_expr_bind(klangc_bare_closure_t *closure, klangc_expr_t *expr) {
  klangc_bare_closure_t *upper;
  klangc_bind_t *bind;
  int cnt_unbound = 0, ret;
  switch (expr->type) {
  case KLANGC_ETYPE_SYMBOL:
    return 0;
  case KLANGC_ETYPE_REF:
    if (klangc_closure_get_bind_by_name(closure, klangc_ref_get_name(expr->ref),
                                        &bind, &upper)) {
      if (expr->ref_ref != NULL) {
        klangc_ipos_print(kstderr, expr->ipos);
        klangc_printf(kstderr,
                      "Variable ~%s is bound to different "
                      "definitions in different scopes",
                      klangc_ref_get_name(expr->ref));
        return -1;
      }
      expr->ref_ref = bind;
      return cnt_unbound;
    }
    cnt_unbound++;
    return cnt_unbound;

  case KLANGC_ETYPE_INT:
  case KLANGC_ETYPE_STRING:
    return 0;

  case KLANGC_ETYPE_APPL:
    ret = klangc_expr_bind(closure, expr->kv_appl->kva_func);
    if (ret < 0)
      return -1;
    cnt_unbound += ret;
    ret = klangc_expr_bind(closure, expr->kv_appl->kva_arg);
    if (ret < 0)
      return -1;
    cnt_unbound += ret;
    break;

  case KLANGC_ETYPE_LAMBDA:
    ret = klangc_expr_bind(klangc_expr_lambda_get_upper(expr->kv_lambda),
                           klangc_expr_lambda_get_body(expr->kv_lambda));
    if (ret < 0)
      return -1;
    cnt_unbound += ret;
    break;

  case KLANGC_ETYPE_CLOSURE:
    ret = klangc_closure_walk_bind(expr->kv_closure, klangc_expr_bind_for_walk,
                                   NULL);
    if (ret < 0)
      return -1;
    cnt_unbound += ret;
    break;
  }
  return cnt_unbound;
}

int klangc_expr_check_unbound(klangc_output_t *output,
                              klangc_bare_closure_t *closure,
                              klangc_expr_t *expr) {
  int cnt_unbound, ret;
  switch (expr->type) {
  case KLANGC_ETYPE_SYMBOL:
    return 0;
  case KLANGC_ETYPE_REF:
    if (expr->ref_ref == NULL) {
      klangc_ipos_print(output, expr->ipos);
      klangc_printf(output, "Unbound variable: ~%s\n",
                    klangc_ref_get_name(expr->ref));
      return 1;
    }
    return 0;

  case KLANGC_ETYPE_INT:
  case KLANGC_ETYPE_STRING:
    return 0;

  case KLANGC_ETYPE_APPL:
    ret = klangc_expr_check_unbound(output, closure, expr->kv_appl->kva_func);
    if (ret < 0)
      return -1;
    cnt_unbound = ret;
    ret = klangc_expr_check_unbound(output, closure, expr->kv_appl->kva_arg);
    if (ret < 0)
      return -1;
    cnt_unbound += ret;
    return cnt_unbound;

  case KLANGC_ETYPE_LAMBDA:
    ret = klangc_expr_check_unbound(
        output, klangc_expr_lambda_get_upper(expr->kv_lambda),
        klangc_expr_lambda_get_body(expr->kv_lambda));
    if (ret < 0)
      return -1;
    cnt_unbound = ret;
    return cnt_unbound;

  case KLANGC_ETYPE_CLOSURE:
    ret = klangc_closure_walk_bind(expr->kv_closure,
                                   klangc_expr_check_unbound_for_walk, output);
    if (ret < 0)
      return -1;
    cnt_unbound = ret;
    return cnt_unbound;
  }
  assert(0);
}

int klangc_expr_check_unbound_for_walk(klangc_bare_closure_t *closure,
                                       klangc_bind_t *bind, void *data) {
  klangc_output_t *output = (klangc_output_t *)data;
  return klangc_expr_check_unbound(output, closure, klangc_bind_get_expr(bind));
}

klangc_ipos_t klangc_expr_get_ipos(klangc_expr_t *expr) { return expr->ipos; }