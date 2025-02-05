#include "expr_closure.h"
#include "bind.h"
#include "expr.h"
#include "expr_env.h"
#include "expr_lambda.h"
#include "input.h"
#include "malloc.h"
#include "output.h"
#include "parse.h"
#include "types.h"
#include <assert.h>

// *******************************
// Closure expression.
// *******************************
// -------------------------------
// Structures.
// -------------------------------
/**
 * Closure expression.
 */
struct klangc_expr_closure {
  /** Expression */
  klangc_expr_t *kc_expr;
  /** Binding */
  klangc_bind_t *kc_bind;
};

// -------------------------------
// Constructors.
// -------------------------------
klangc_expr_closure_t *klangc_expr_closure_new(klangc_expr_t *expr,
                                               klangc_bind_t *bind) {
  klangc_expr_closure_t *closure = klangc_malloc(sizeof(klangc_expr_closure_t));
  closure->kc_expr = expr;
  closure->kc_bind = bind;
  return closure;
}

// -------------------------------
// Accessors.
// -------------------------------
klangc_expr_t *klangc_expr_closure_get_expr(klangc_expr_closure_t *closure) {
  return closure->kc_expr;
}

klangc_bind_t *klangc_expr_closure_get_bind(klangc_expr_closure_t *closure) {
  return closure->kc_bind;
}

static int klangc_expr_closure_is_multiline(klangc_expr_closure_t *closure) {
  if (closure->kc_bind != NULL)
    return 1;
  klangc_expr_type_t type = klangc_expr_get_type(closure->kc_expr);
  if (type != KLANGC_ETYPE_LAMBDA)
    return 1;
  klangc_expr_lambda_t *lambda = klangc_expr_get_lambda(closure->kc_expr);
  klangc_expr_lambda_t *lambda_next = klangc_expr_lambda_get_next(lambda);
  if (lambda_next != NULL)
    return 1;
  return 0;
}

// -------------------------------
// Parsers.
// -------------------------------
klangc_parse_result_t
klangc_expr_closure_parse_nobrace(klangc_input_t *input,
                                  klangc_expr_closure_t **pclosure) {
  assert(input != NULL);
  assert(pclosure != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  klangc_expr_t *expr = NULL;
  klangc_parse_result_t res =
      klangc_expr_parse(input, KLANGC_EXPR_PARSE_NORMAL, &expr);
  if (res != KLANGC_PARSE_OK)
    return res;

  klangc_ipos_t ipos2 = klangc_input_save(input);
  ipos_ss = klangc_skipspaces(input);
  int c = EOF;
  res = klangc_expect(input, ';', &c);
  if (res != KLANGC_PARSE_OK) {
    klangc_printf_ipos_expects(
        kstderr, ipos_ss, "';'", c,
        "<closure_bare> ::= <expr> ^';' ( <bind> ';' )?;\n");
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  klangc_bind_t *bind = NULL;
  ipos_ss = klangc_skipspaces(input);
  switch (klangc_bind_parse(input, &bind)) {
  case KLANGC_PARSE_OK:
    break;

  case KLANGC_PARSE_NOPARSE:
    *pclosure = klangc_expr_closure_new(expr, bind);
    return KLANGC_PARSE_OK;

  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  ipos_ss = klangc_skipspaces(input);
  res = klangc_expect(input, ';', &c);
  if (res != KLANGC_PARSE_OK) {
    klangc_printf_ipos_expects(kstderr, ipos_ss, "';'", c,
                               "<closure> ::= <expr> ';' ( <bind> ^';' )?;\n");
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  *pclosure = klangc_expr_closure_new(expr, bind);
  return KLANGC_PARSE_OK;
}

klangc_parse_result_t
klangc_expr_closure_parse(klangc_input_t *input,
                          klangc_expr_closure_t **pclosure) {
  assert(input != NULL);
  assert(pclosure != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_parse_result_t res = klangc_expect(input, '{', NULL);
  if (res != KLANGC_PARSE_OK) {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  klangc_expr_closure_t *closure = NULL;
  klangc_ipos_t ipos_ss = klangc_skipspaces(input);
  res = klangc_expr_closure_parse_nobrace(input, &closure);
  switch (res) {
  case KLANGC_PARSE_OK:
    break;
  case KLANGC_PARSE_NOPARSE:
    klangc_printf_ipos_expects(kstderr, ipos_ss, "<expr>", klangc_getc(input),
                               "<closure> ::= '{' ^<closure_bare> '}';\n");
  case KLANGC_PARSE_ERROR:
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }

  int c = EOF;
  ipos_ss = klangc_skipspaces(input);
  res = klangc_expect(input, '}', &c);
  if (res != KLANGC_PARSE_OK) {
    klangc_printf_ipos_expects(kstderr, ipos_ss, "}", c,
                               "<closure> ::= '{' <closure_bare> ^'}'\n");
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_ERROR;
  }
  *pclosure = closure;
  return KLANGC_PARSE_OK;
}

// -------------------------------
// Printers.
// -------------------------------
void klangc_expr_closure_print(klangc_output_t *output,
                               klangc_expr_closure_t *closure) {

  if (!klangc_expr_closure_is_multiline(closure)) {
    klangc_printf(output, "{ ");
    klangc_expr_print(output, KLANGC_PREC_LOWEST, closure->kc_expr);
    klangc_printf(output, " }");
    return;
  }
  klangc_printf(output, "{\n");
  klangc_indent(output, 2);
  klangc_expr_print(output, KLANGC_PREC_LOWEST, closure->kc_expr);
  klangc_printf(output, ";\n");
  if (closure->kc_bind != NULL) {
    klangc_bind_print(output, closure->kc_bind);
    klangc_printf(output, ";\n");
  }
  klangc_indent(output, -2);
  klangc_printf(output, "}");
}

void klangc_expr_closure_print_nobrace(klangc_output_t *output,
                                       klangc_expr_closure_t *closure) {

  if (!klangc_expr_closure_is_multiline(closure)) {
    klangc_expr_print(output, KLANGC_PREC_LOWEST, closure->kc_expr);
    klangc_printf(output, ";\n");
    return;
  }
  klangc_expr_print(output, KLANGC_PREC_LOWEST, closure->kc_expr);
  klangc_printf(output, ";\n");
  if (closure->kc_bind != NULL) {
    klangc_bind_print(output, closure->kc_bind);
    klangc_printf(output, ";\n");
  }
}

// -------------------------------
// Binders.
// -------------------------------
klangc_bind_result_t klangc_expr_closure_bind(klangc_expr_env_t *env,
                                              klangc_expr_closure_t *closure) {
  klangc_expr_env_t *env_inner = klangc_expr_env_new(env);
  if (closure->kc_bind != NULL)
    if (klangc_bind_bind(env_inner, closure->kc_bind) == -1)
      return -1;
  return klangc_expr_bind(env_inner, closure->kc_expr);
}
