#include "klangc_expr.h"
#include "klangc_bind.h"
#include "klangc_closure.h"
#include "klangc_eclosure.h"
#include "klangc_elambda.h"
#include "klangc_input.h"
#include "klangc_malloc.h"
#include "klangc_message.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_pattern.h"
#include "klangc_types.h"

typedef enum klangc_expr_type {
  KLANGC_ETYPE_SYMBOL,
  KLANGC_ETYPE_APPL,
  KLANGC_ETYPE_INT,
  KLANGC_ETYPE_STRING,
  KLANGC_ETYPE_LAMBDA,
  KLANGC_ETYPE_CLOSURE,
  KLANGC_ETYPE_CHOICE,
} klangc_expr_type_t;

typedef struct klangc_expr_appl klangc_expr_appl_t;
typedef struct klangc_expr_choice klangc_expr_choice_t;

klangc_parse_result_t klangc_expr_parse_no_choice(klangc_input_t *input,
                                                  klangc_closure_t *enclosed_by,
                                                  klangc_expr_t **pexpr);

struct klangc_expr_appl {
  klangc_expr_t *kva_func;
  klangc_expr_t *kva_arg;
};

struct klangc_expr_choice {
  klangc_expr_t *kvc_base;
  klangc_expr_t *kvc_otherwise;
};

struct klangc_expr {
  klangc_expr_type_t type;
  union {
    struct {
      char *symbol;
      klangc_bind_t *symbol_ref;
    };
    int intval;
    char *strval;
    klangc_expr_appl_t *kv_appl;
    klangc_expr_lambda_t *kv_lambda;
    klangc_expr_closure_t *kv_eclosure;
    klangc_expr_choice_t *kv_choice;
  };
  klangc_ipos_t ipos;
};

int klangc_expr_is_choice(klangc_expr_t *expr) {
  return expr->type == KLANGC_ETYPE_CHOICE;
}

klangc_expr_appl_t *klangc_expr_appl_new(klangc_expr_t *func,
                                         klangc_expr_t *arg) {
  assert(func != NULL);
  assert(arg != NULL);
  klangc_expr_appl_t *ret = klangc_malloc(sizeof(klangc_expr_appl_t));
  ret->kva_func = func;
  ret->kva_arg = arg;
  return ret;
}

klangc_expr_choice_t *klangc_expr_choice_new(klangc_expr_t *base,
                                             klangc_expr_t *otherwise) {
  klangc_expr_choice_t *ret = klangc_malloc(sizeof(klangc_expr_choice_t));
  ret->kvc_base = base;
  ret->kvc_otherwise = otherwise;
  return ret;
}

klangc_expr_t *klangc_expr_new_symbol(const char *symbol, klangc_ipos_t ipos) {
  assert(symbol != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_SYMBOL;
  expr->symbol = klangc_strdup(symbol);
  expr->symbol_ref = NULL;
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

klangc_expr_t *klangc_expr_new_lambda(klangc_expr_lambda_t *lambda,
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

klangc_expr_t *klangc_expr_new_eclosure(klangc_expr_closure_t *eclosure,
                                        klangc_ipos_t ipos) {
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_CLOSURE;
  expr->kv_eclosure = eclosure;
  expr->ipos = ipos;
  return expr;
}

klangc_expr_t *klangc_expr_new_choice(klangc_expr_choice_t *choice,
                                      klangc_ipos_t ipos) {
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_CHOICE;
  expr->kv_choice = choice;
  expr->ipos = ipos;
  return expr;
}

klangc_parse_result_t klangc_expr_lambda_parse(klangc_input_t *input,
                                               klangc_closure_t *upper,
                                               klangc_expr_lambda_t **plambda) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (c != '\\') {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  ipos_ss = klangc_skipspaces(input);
  klangc_pattern_t *arg;
  switch (klangc_pattern_parse(input, &arg)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_print_ipos(kstderr, ipos_ss);
    klangc_printf(
        kstderr,
        "expect <pattern>: ['\\' ^<pattern> '->' <expr(w/o choice)>]\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss = klangc_skipspaces(input);
  c = klangc_getc(input);
  if (c != '-') {
    klangc_print_ipos(kstderr, ipos_ss);
    klangc_printf(
        kstderr,
        "expect '-' but get '%c': ['\\' <pattern> ^'->' <expr(w/o choice)>]\n",
        c);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  c = klangc_getc(input);
  if (c != '>') {
    klangc_print_ipos(kstderr, ipos_ss);
    klangc_printf(kstderr,
                  "expect '->' but get '-%c': ['\\' <pattern> ^'->' "
                  "<expr(w/o choice)>]\n",
                  c);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss = klangc_skipspaces(input);
  klangc_expr_t *body;
  switch (klangc_expr_parse_no_choice(input, upper, &body)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_print_ipos(kstderr, ipos_ss);
    klangc_printf(kstderr,
                  "expect <expr>: ['\\' <pattern> '->' ^<expr(w/o choice)>]\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  *plambda = klangc_expr_lambda_new(arg, body, upper, ipos_ss);
  return KLANGC_PARSE_OK;
}

klangc_parse_result_t klangc_expr_parse_no_appl(klangc_input_t *input,
                                                klangc_closure_t *enclosed_by,
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
      klangc_print_ipos(kstderr, ipos_ss);
      klangc_printf(kstderr, "expect <expr>: ['(' ^<expr> ')']\n");
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }

    ipos_ss = klangc_skipspaces(input);
    c = klangc_getc(input);
    if (c != ')') {
      klangc_print_ipos(kstderr, ipos_ss);
      klangc_printf(kstderr, "expect ')' but get '%c': ['(' <expr> ^')']\n", c);
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    *pexpr = expr;
    return KLANGC_PARSE_OK;
  } while (0);

  klangc_input_restore(input, ipos_ss);

  klangc_expr_closure_t *eclosure;
  switch (klangc_expr_closure_parse(input, enclosed_by, &eclosure)) {
  case KLANGC_PARSE_OK:
    *pexpr = klangc_expr_new_eclosure(eclosure, ipos_ss);
    return KLANGC_PARSE_OK;
  case KLANGC_PARSE_NOPARSE:
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  char *symbol;
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

  klangc_expr_lambda_t *lambda;
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

klangc_parse_result_t klangc_expr_parse_no_choice(klangc_input_t *input,
                                                  klangc_closure_t *enclosed_by,
                                                  klangc_expr_t **pexpr) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_expr_t *expr;
  switch (klangc_expr_parse_no_appl(input, enclosed_by, &expr)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_print_ipos(kstderr, ipos_ss);
    klangc_printf(kstderr, "expect <expr(w/o appl)>: [^<expr(w/o appl)>]\n");
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

klangc_parse_result_t klangc_expr_parse(klangc_input_t *input,
                                        klangc_closure_t *enclosed_by,
                                        klangc_expr_t **pexpr) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_expr_t *expr;
  switch (klangc_expr_parse_no_choice(input, enclosed_by, &expr)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_print_ipos(kstderr, ipos_ss);
    klangc_printf(
        kstderr,
        "expect <expr(w/o choice)>: [^<expr(w/o choice)> ('|' <expr>)?]\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  klangc_ipos_t ipos2 = klangc_input_save(input);
  klangc_ipos_t ipos_ss2 = klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (c != '|') {
    klangc_input_restore(input, ipos2);
    *pexpr = expr;
    return KLANGC_PARSE_OK;
  }
  klangc_expr_t *base = expr;
  klangc_expr_t *otherwise;
  ipos2 = klangc_input_save(input);
  ipos_ss2 = klangc_skipspaces(input);
  switch (klangc_expr_parse(input, enclosed_by, &otherwise)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_print_ipos(kstderr, ipos_ss2);
    klangc_printf(kstderr,
                  "expect <expr>: [^<expr(w/o choice)> ('|' ^<expr>)?]\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_expr_choice_t *choice = klangc_expr_choice_new(base, otherwise);
  *pexpr = klangc_expr_new_choice(choice, ipos_ss2);
  return KLANGC_PARSE_OK;
}

void klangc_expr_choice_print(klangc_output_t *output, int prec,
                              klangc_expr_choice_t *choice, int chain) {
  if (prec > KLANGC_PREC_CHOICE)
    klangc_printf(output, "(");
  if (!chain) {
    klangc_indent(output, 2);
    klangc_printf(output, "\n  ");
  }
  klangc_expr_print(output, KLANGC_PREC_CHOICE + 1, choice->kvc_base);
  klangc_printf(output, "\n| ");
  if (klangc_expr_is_choice(choice->kvc_otherwise))
    klangc_expr_choice_print(output, KLANGC_PREC_CHOICE,
                             choice->kvc_otherwise->kv_choice, 1);
  else
    klangc_expr_print(output, KLANGC_PREC_CHOICE, choice->kvc_otherwise);
  if (!chain) {
    klangc_indent(output, -2);
    klangc_printf(output, "\n");
  }
  if (prec > KLANGC_PREC_CHOICE)
    klangc_printf(output, ")");
}

void klangc_expr_print(klangc_output_t *output, int prec, klangc_expr_t *expr) {
  switch (expr->type) {
  case KLANGC_ETYPE_SYMBOL:
    klangc_printf(output, "%s", expr->symbol);
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
    klangc_expr_closure_print(output, expr->kv_eclosure);
    break;
  case KLANGC_ETYPE_CHOICE:
    klangc_expr_choice_print(output, prec, expr->kv_choice, 0);
    break;
  }
}

int klangc_expr_bind_by_walk(klangc_closure_t *closure, klangc_pattern_t *pat,
                             klangc_expr_t *expr, void *data) {
  return klangc_expr_bind(closure, expr);
}

int klangc_expr_bind(klangc_closure_t *closure, klangc_expr_t *expr) {
  klangc_closure_t *upper;
  klangc_bind_t *bind;
  int cnt_unbound = 0, ret;
  switch (expr->type) {
  case KLANGC_ETYPE_SYMBOL:
    if (klangc_closure_get_bind_by_name(closure, expr->symbol, &bind, &upper)) {
      if (expr->symbol_ref != NULL) {
        klangc_print_ipos(kstderr, expr->ipos);
        klangc_printf(kstderr,
                      "Variable %s is bound to different "
                      "definitions in different scopes",
                      expr->symbol);
        return -1;
      }
      expr->symbol_ref = bind;
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
    ret = klangc_closure_walk_bind(
        klangc_expr_closure_get_closure(expr->kv_eclosure),
        klangc_expr_bind_by_walk, NULL);
    if (ret < 0)
      return -1;
    cnt_unbound += ret;
    break;

  case KLANGC_ETYPE_CHOICE:
    ret = klangc_expr_bind(closure, expr->kv_choice->kvc_base);
    if (ret < 0)
      return -1;
    cnt_unbound += ret;
    ret = klangc_expr_bind(closure, expr->kv_choice->kvc_otherwise);
    if (ret < 0)
      return -1;
    cnt_unbound += ret;
    break;
  }
  return cnt_unbound;
}

int klangc_expr_check_unbound(klangc_output_t *output,
                              klangc_closure_t *closure, klangc_expr_t *expr) {
  int cnt_unbound, ret;
  switch (expr->type) {
  case KLANGC_ETYPE_SYMBOL:
    if (expr->symbol_ref == NULL) {
      klangc_print_ipos(output, expr->ipos);
      klangc_printf(output, "Unbound variable: %s\n", expr->symbol);
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
    ret = klangc_closure_walk_bind(
        klangc_expr_closure_get_closure(expr->kv_eclosure),
        klangc_expr_check_unbound_by_walk, output);
    if (ret < 0)
      return -1;
    cnt_unbound = ret;
    return cnt_unbound;

  case KLANGC_ETYPE_CHOICE:
    ret = klangc_expr_check_unbound(output, closure, expr->kv_choice->kvc_base);
    if (ret < 0)
      return -1;
    cnt_unbound = ret;
    ret = klangc_expr_check_unbound(output, closure,
                                    expr->kv_choice->kvc_otherwise);
    if (ret < 0)
      return -1;
    cnt_unbound += ret;
    return cnt_unbound;
  }
  assert(0);
}

int klangc_expr_check_unbound_by_walk(klangc_closure_t *closure,
                                      klangc_bind_t *bind, void *data) {
  klangc_output_t *output = (klangc_output_t *)data;
  return klangc_expr_check_unbound(output, closure, klangc_bind_get_expr(bind));
}

klangc_ipos_t klangc_expr_get_ipos(klangc_expr_t *expr) { return expr->ipos; }