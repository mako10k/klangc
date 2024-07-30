#include "klangc_expr.h"
#include "klangc.h"
#include "klangc_input.h"
#include "klangc_malloc.h"
#include "klangc_parse.h"
#include "klangc_pattern.h"

typedef enum klangc_expr_type {
  KLANGC_ETYPE_SYMBOL,
  KLANGC_ETYPE_APPL,
  KLANGC_ETYPE_INT,
  KLANGC_ETYPE_STRING,
  KLANGC_ETYPE_LAMBDA,
} klangc_expr_type_t;

typedef struct klangc_expr_lambda {
  klangc_pattern_t *kvl_arg;
  klangc_expr_t *kvl_expr;
} klangc_expr_lambda_t;

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
  };
};

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

klangc_expr_t *klangc_expr_string_new(const char *strval) {
  assert(strval != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_STRING;
  expr->strval = klangc_strdup(strval);
  return expr;
}

klangc_expr_t *klangc_expr_lambda_new(klangc_pattern_t *arg,
                                      klangc_expr_t *expr) {
  assert(arg != NULL);
  assert(expr != NULL);
  klangc_expr_t *ret = klangc_malloc(sizeof(klangc_expr_t));
  ret->type = KLANGC_ETYPE_LAMBDA;
  ret->kv_lambda = klangc_malloc(sizeof(klangc_expr_lambda_t));
  ret->kv_lambda->kvl_arg = arg;
  ret->kv_lambda->kvl_expr = expr;
  return ret;
}

klangc_expr_t *klangc_expr_appl_new(klangc_expr_t *func, klangc_expr_t *arg) {
  assert(func != NULL);
  assert(arg != NULL);
  klangc_expr_t *expr = klangc_malloc(sizeof(klangc_expr_t));
  expr->type = KLANGC_ETYPE_APPL;
  expr->kv_appl = klangc_malloc(sizeof(klangc_expr_appl_t));
  expr->kv_appl->kva_func = func;
  expr->kv_appl->kva_arg = arg;
  return expr;
}

klangc_expr_t *klangc_expr_lambda_parse(klangc_input_t *input) {
  klangc_input_buf_t ib = klangc_input_save(input);
  int c = klangc_getc_skipspaces(input);
  if (c != '\\') {
    klangc_input_restore(input, ib);
    return NULL;
  }
  klangc_pattern_t *arg = klangc_pattern_parse(input);
  if (arg == NULL) {
    klangc_input_restore(input, ib);
    return NULL;
  }
  c = klangc_getc_skipspaces(input);
  if (c != '-') {
    klangc_input_restore(input, ib);
    return NULL;
  }
  c = klangc_getc(input);
  if (c != '>') {
    klangc_input_restore(input, ib);
    return NULL;
  }
  klangc_expr_t *body = klangc_expr_parse(input);
  if (body == NULL) {
    klangc_input_restore(input, ib);
    return NULL;
  }
  return klangc_expr_lambda_new(arg, body);
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
    if (expr == NULL)
      break;
    c = klangc_getc_skipspaces(input);
    if (c != ')')
      break;
    return expr;
  } while (0);
  klangc_input_restore(input, ib);

  char *symbol = klangc_symbol_parse(input);
  if (symbol != NULL)
    return klangc_expr_symbol_new(symbol);

  int intval;
  if (klangc_int_parse(input, &intval))
    return klangc_expr_int_new(intval);

  char *strval = klangc_string_parse(input);
  if (strval != NULL)
    return klangc_expr_string_new(strval);

  klangc_expr_t *lambda = klangc_expr_lambda_parse(input);
  if (lambda != NULL)
    return lambda;

  return NULL;
}

klangc_expr_t *klangc_expr_parse(klangc_input_t *input) {
  klangc_expr_t *ret = klangc_expr_parse_no_appl(input);
  if (ret == NULL)
    return NULL;
  while (1) {
    klangc_expr_t *arg = klangc_expr_parse_no_appl(input);
    if (arg == NULL)
      return ret;
    ret = klangc_expr_appl_new(ret, arg);
  }
  return ret;
}

void klangc_expr_print(FILE *fp, klangc_expr_t *expr) {
  switch (expr->type) {
  case KLANGC_ETYPE_SYMBOL:
    fprintf(fp, "%s", expr->symbol);
    break;
  case KLANGC_ETYPE_INT:
    fprintf(fp, "%d", expr->intval);
    break;
  case KLANGC_ETYPE_STRING:
    fprintf(fp, "\"%s\"", expr->strval);
    break;
  case KLANGC_ETYPE_APPL:
    fprintf(fp, "(");
    klangc_expr_print(fp, expr->kv_appl->kva_func);
    fprintf(fp, " ");
    klangc_expr_print(fp, expr->kv_appl->kva_arg);
    fprintf(fp, ")");
    break;
  case KLANGC_ETYPE_LAMBDA:
    fprintf(fp, "(\\");
    klangc_pattern_print(fp, expr->kv_lambda->kvl_arg);
    fprintf(fp, " -> ");
    klangc_expr_print(fp, expr->kv_lambda->kvl_expr);
    fprintf(fp, ")");
    break;
  }
}
