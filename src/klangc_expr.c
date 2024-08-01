#include "klangc_expr.h"
#include "klangc_dict.h"
#include "klangc_input.h"
#include "klangc_malloc.h"
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
  KLANGC_ETYPE_DICT
} klangc_expr_type_t;

typedef struct klangc_expr_lambda klangc_expr_lambda_t;

typedef struct klangc_expr_appl {
  klangc_expr_t *kva_func;
  klangc_expr_t *kva_arg;
} klangc_expr_appl_t;

struct klangc_expr {
  klangc_expr_type_t type;
  union {
    char *symbol;
    int intval;
    char *strval;
    klangc_expr_appl_t *kv_appl;
    klangc_expr_lambda_t *kv_lambda;
    klangc_dict_t *kv_dict;
  };
  klangc_ipos_t ipos;
};

struct klangc_expr_lambda {
  klangc_pattern_t *kvl_arg;
  klangc_expr_t *kvl_expr;
  klangc_expr_lambda_t *kvl_next;
};

klangc_expr_lambda_t *klangc_expr_lambda_new(klangc_pattern_t *arg,
                                             klangc_expr_t *expr) {
  assert(arg != NULL);
  assert(expr != NULL);
  klangc_expr_lambda_t *ret = klangc_malloc(sizeof(klangc_expr_lambda_t));
  ret->kvl_arg = arg;
  ret->kvl_expr = expr;
  ret->kvl_next = NULL;
  return ret;
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

klangc_expr_t *klangc_expr_symbol_new(const char *symbol, klangc_ipos_t ipos) {
  assert(symbol != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_SYMBOL;
  expr->symbol = klangc_strdup(symbol);
  expr->ipos = ipos;
  return expr;
}

klangc_expr_t *klangc_expr_int_new(int intval, klangc_ipos_t ipos) {
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

klangc_parse_result_t klangc_expr_lambda_parse(klangc_input_t *input,
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
    klangc_message_reset(input);
    klangc_message_add_ipos(input, ipos_ss);
    klangc_message_add(input,
                       "expect <pattern>: ['\\' ^<pattern> '->' <expr>]\n");
    klangc_message_print(input, kstderr);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss = klangc_skipspaces(input);
  c = klangc_getc(input);
  if (c != '-') {
    klangc_message_reset(input);
    klangc_message_add_ipos(input, ipos_ss);
    klangc_message_add(
        input, "expect '-' but get '%c': ['\\' <pattern> ^'->' <expr>]\n", c);
    klangc_message_print(input, kstderr);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  c = klangc_getc(input);
  if (c != '>') {
    klangc_message_reset(input);
    klangc_message_add_ipos(input, ipos_ss);
    klangc_message_add(
        input, "expect '->' but get '-%c': ['\\' <pattern> ^'->' <expr>]\n", c);
    klangc_message_print(input, kstderr);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  ipos_ss = klangc_skipspaces(input);
  klangc_expr_t *body;
  switch (klangc_expr_parse(input, &body)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_message_reset(input);
    klangc_message_add_ipos(input, ipos_ss);
    klangc_message_add(input, "expect <expr>: ['\\' <pattern> '->' ^<expr>]\n");
    klangc_message_print(input, kstderr);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_expr_lambda_t *lambda = klangc_expr_lambda_new(arg, body);

  ipos_ss = klangc_input_save(input);
  klangc_ipos_t ipos3 = klangc_skipspaces(input);
  (void)ipos3;
  c = klangc_getc(input);
  if (c != '|') {
    klangc_input_restore(input, ipos_ss);
    *plambda = lambda;
    return KLANGC_PARSE_OK;
  }
  lambda->kvl_next = NULL;
  switch (klangc_expr_lambda_parse(input, &lambda->kvl_next)) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_input_restore(input, ipos_ss);
    break;
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  *plambda = lambda;
  return KLANGC_PARSE_OK;
}

klangc_expr_t *klangc_expr_new_dict(klangc_dict_t *def) {
  klangc_expr_t *dict = klangc_malloc(sizeof(klangc_expr_t));
  dict->type = KLANGC_ETYPE_DICT;
  dict->kv_dict = def;
  return dict;
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
      klangc_message_reset(input);
      klangc_message_add_ipos(input, ipos_ss);
      klangc_message_add(input, "expect <expr>: ['(' ^<expr> ')']\n");
      klangc_message_print(input, kstderr);
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    case KLANGC_PARSE_ERROR:
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }

    ipos_ss = klangc_skipspaces(input);
    c = klangc_getc(input);
    if (c != ')') {
      klangc_message_reset(input);
      klangc_message_add_ipos(input, ipos_ss);
      klangc_message_add(input, "expect ')' but get '%c': ['(' <expr> ^')']\n",
                         c);
      klangc_message_print(input, kstderr);
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    *pexpr = expr;
    return KLANGC_PARSE_OK;
  } while (0);

  klangc_input_restore(input, ipos_ss);

  klangc_dict_t *dict;
  switch (klangc_dict_parse(input, &dict)) {
  case KLANGC_PARSE_OK:
    *pexpr = klangc_expr_new_dict(dict);
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
    *pexpr = klangc_expr_symbol_new(symbol, ipos_ss);
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
    *pexpr = klangc_expr_int_new(intval, ipos_ss);
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
    klangc_message_reset(input);
    klangc_message_add_ipos(input, ipos_ss);
    klangc_message_add(input, "expect <expr(w/o appl)>: [^<expr(w/o appl)>]\n");
    klangc_message_print(input, kstderr);
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
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

void klangc_expr_lambda_print(klangc_output_t *output,
                              klangc_expr_lambda_t *lambda) {
  if (lambda->kvl_next == NULL) {
    klangc_printf(output, "\\");
    klangc_pattern_print(output, KLANGC_PREC_LOWEST, lambda->kvl_arg);
    klangc_printf(output, " -> ");
    klangc_expr_print(output, KLANGC_PREC_LOWEST, lambda->kvl_expr);
    return;
  }
  klangc_printf(output, "\n");
  klangc_indent(output, 2);
  do {
    klangc_printf(output, "\\");
    klangc_pattern_print(output, KLANGC_PREC_LOWEST, lambda->kvl_arg);
    klangc_printf(output, " -> ");
    klangc_expr_print(output, KLANGC_PREC_LOWEST, lambda->kvl_expr);
    if (lambda->kvl_next != NULL)
      klangc_printf(output, " |\n");
    lambda = lambda->kvl_next;
  } while (lambda != NULL);
  klangc_indent(output, -2);
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
  case KLANGC_ETYPE_DICT:
    klangc_dict_print(output, expr->kv_dict);
    break;
  }
}
