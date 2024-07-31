#include "klangc_expr.h"
#include "klangc_dict.h"
#include "klangc_input.h"
#include "klangc_malloc.h"
#include "klangc_output.h"
#include "klangc_parse.h"
#include "klangc_pattern.h"

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

klangc_expr_t *klangc_expr_symbol_new(const char *symbol) {
  assert(symbol != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_SYMBOL;
  expr->symbol = klangc_strdup(symbol);
  return expr;
}

klangc_expr_t *klangc_expr_int_new(int intval) {
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_INT;
  expr->intval = intval;
  return expr;
}

klangc_expr_t *klangc_expr_new_string(const char *strval) {
  assert(strval != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_STRING;
  expr->strval = klangc_strdup(strval);
  return expr;
}

klangc_expr_t *klangc_expr_new_lambda(klangc_expr_lambda_t *lambda) {
  assert(lambda != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_LAMBDA;
  expr->kv_lambda = lambda;
  return expr;
}

klangc_expr_t *klangc_expr_new_appl(klangc_expr_appl_t *appl) {
  assert(appl != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_APPL;
  expr->kv_appl = appl;
  return expr;
}

klangc_expr_lambda_t *klangc_expr_lambda_parse(klangc_input_t *input) {
  klangc_input_buf_t ib = klangc_input_save(input);
  int c = klangc_getc_skipspaces(input);
  if (c != '\\') {
    klangc_input_restore(input, ib);
    return NULL;
  }
  klangc_pattern_t *arg = klangc_pattern_parse(input);
  if (arg == NULL) {
    klangc_message_reset(input);
    klangc_message_add_buf(input, NULL);
    klangc_message_add(input,
                       "expect <pattern>: ['\\' ^<pattern> '->' <expr>]\n");
    klangc_message_print(input, kstderr);
    klangc_input_restore(input, ib);
    return NULL;
  }
  c = klangc_getc_skipspaces(input);
  if (c != '-') {
    klangc_message_reset(input);
    klangc_message_add_buf(input, NULL);
    klangc_message_add(
        input, "expect '-' but get '%c': ['\\' <pattern> ^'->' <expr>]\n", c);
    klangc_message_print(input, kstderr);
    klangc_input_restore(input, ib);
    return NULL;
  }
  c = klangc_getc(input);
  if (c != '>') {
    klangc_message_reset(input);
    klangc_message_add_buf(input, NULL);
    klangc_message_add(
        input, "expect '->' but get '-%c': ['\\' <pattern> ^'->' <expr>]\n", c);
    klangc_message_print(input, kstderr);
    klangc_input_restore(input, ib);
    return NULL;
  }
  klangc_expr_t *body = klangc_expr_parse(input);
  if (body == NULL) {
    klangc_message_reset(input);
    klangc_message_add_buf(input, NULL);
    klangc_message_add(input, "expect <expr>: ['\\' <pattern> '->' ^<expr>]\n");
    klangc_message_print(input, kstderr);
    klangc_input_restore(input, ib);
    return NULL;
  }
  klangc_expr_lambda_t *lambda = klangc_expr_lambda_new(arg, body);
  ib = klangc_input_save(input);
  c = klangc_getc_skipspaces(input);
  if (c != '|') {
    klangc_input_restore(input, ib);
    return lambda;
  }
  lambda->kvl_next = klangc_expr_lambda_parse(input);
  if (lambda->kvl_next == NULL)
    klangc_input_restore(input, ib);
  return lambda;
}

klangc_expr_t *klangc_expr_new_dict(klangc_dict_t *def) {
  klangc_expr_t *dict = klangc_malloc(sizeof(klangc_expr_t));
  dict->type = KLANGC_ETYPE_DICT;
  dict->kv_dict = def;
  return dict;
}

klangc_expr_t *klangc_expr_parse_no_appl(klangc_input_t *input) {
  assert(input != NULL);
  klangc_expr_t *expr;

  klangc_input_buf_t ib = klangc_input_save(input);
  do {
    int c = klangc_getc_skipspaces(input);
    if (c != '(')
      break;
    expr = klangc_expr_parse(input);
    if (expr == NULL) {
      klangc_message_reset(input);
      klangc_message_add_buf(input, NULL);
      klangc_message_add(input, "expect <expr>: ['(' ^<expr> ')']\n");
      klangc_message_print(input, kstderr);
      klangc_input_restore(input, ib);
      return NULL;
    }
    c = klangc_getc_skipspaces(input);
    if (c != ')') {
      klangc_message_reset(input);
      klangc_message_add_buf(input, NULL);
      klangc_message_add(input, "expect ')' but get '%c': ['(' <expr> ^')']\n",
                         c);
      klangc_message_print(input, kstderr);
      klangc_input_restore(input, ib);
      return NULL;
    }
    return expr;
  } while (0);
  klangc_input_restore(input, ib);

  klangc_dict_t *dict = klangc_dict_parse(input);
  if (dict != NULL)
    return klangc_expr_new_dict(dict);

  klangc_input_restore(input, ib);

  char *symbol = klangc_symbol_parse(input);
  if (symbol != NULL)
    return klangc_expr_symbol_new(symbol);

  int intval;
  if (klangc_int_parse(input, &intval))
    return klangc_expr_int_new(intval);

  char *strval = klangc_string_parse(input);
  if (strval != NULL)
    return klangc_expr_new_string(strval);

  klangc_expr_lambda_t *lambda = klangc_expr_lambda_parse(input);
  if (lambda != NULL) {
    expr = klangc_expr_new_lambda(lambda);
    return expr;
  }

  return NULL;
}

klangc_expr_t *klangc_expr_parse(klangc_input_t *input) {
  klangc_expr_t *ret = klangc_expr_parse_no_appl(input);
  if (ret == NULL) {
    klangc_message_reset(input);
    klangc_message_add_buf(input, NULL);
    klangc_message_add(input,
                       "expect <expr>: [^'(' <expr> ')' | ^'{' <defs> '}' | "
                       "^'\\' <pattern> '->' <body> | ^<expr>+ ]\n");
    klangc_message_print(input, kstderr);
    return NULL;
  }
  while (1) {
    klangc_expr_t *arg = klangc_expr_parse_no_appl(input);
    if (arg == NULL)
      return ret;
    klangc_expr_appl_t *appl = klangc_expr_appl_new(ret, arg);
    ret = klangc_expr_new_appl(appl);
  }
  return ret;
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
